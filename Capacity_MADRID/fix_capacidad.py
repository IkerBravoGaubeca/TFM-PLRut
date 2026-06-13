"""
Script que recorre todos los partialsX.json de cada cuadrante hasta el escenario indicado
y para cada tramo coge el número de vehículos del último escenario NO saturado.
Genera el JSON actualizado y una imagen con colormap RdYlGn.
"""

import json
import os
import re
import argparse
import sumolib
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import matplotlib.cm as cm
from shapely.geometry import Point, Polygon


def read_stations(file_path):
    stations = []
    with open(file_path, 'r') as file:
        for line in file:
            match = re.search(r'p=(\d+\.\d+),(\d+\.\d+)', line)
            if match:
                x, y = float(match.group(1)), float(match.group(2))
                stations.append((x, y))
    print(f"Se encontraron {len(stations)} estaciones")
    return stations


def convert_geojson_to_sumo(geojson_coords, net):
    sumo_coords = []
    map_height = net.getBBoxXY()[1][1]
    for lon, lat in geojson_coords:
        x, y = net.convertLonLat2XY(lon, lat)
        y = map_height - y
        sumo_coords.append((x, y))
    return sumo_coords


def generate_heatmap(capacidad, net, stations, geojson_coords, output_image, escenario, base_radius=150):
    print(f"Generando mapa de calor en {output_image}")
    y_max = net.getBBoxXY()[1][1]

    area_polygon = convert_geojson_to_sumo(geojson_coords, net)
    polygon = Polygon(area_polygon)

    norm = mcolors.Normalize(vmin=0, vmax=13)
    cmap = plt.get_cmap("RdYlGn")

    fig, ax = plt.subplots(figsize=(12, 12), dpi=300)

    # 1. Dibujar todos los tramos en negro de base
    for edge in net.getEdges():
        road = edge.getShape()
        x_road, y_road = zip(*road)
        inverted_y = [y_max - y for y in y_road]
        plt.plot(x_road, inverted_y, color="black", linewidth=1, zorder=1)

    # 2. Dibujar estaciones base
    for x, y in stations:
        circle = plt.Circle((x, y), base_radius, color='blue', fill=True, alpha=0.03, linestyle='dotted', zorder=3)
        ax.add_artist(circle)
        plt.plot(x, y, 'bo', markersize=5, zorder=4)

    # 3. Dibujar tramos con vehiculos con colormap
    total_vehicles_map = 0
    for edge in net.getEdges():
        edge_id = edge.getID()
        road = edge.getShape()
        x_road, y_road = zip(*road)
        inverted_y = [y_max - y for y in y_road]

        x1, y1 = road[0]
        x2, y2 = road[-1]
        mid_x = (x1 + x2) / 2
        mid_y = (y1 + y2) / 2
        inverted_mid_y = y_max - mid_y
        mid_point = Point(mid_x, inverted_mid_y)

        if polygon.contains(mid_point) and edge_id in capacidad:
            cap_data = capacidad[edge_id]
            vehicles = cap_data.get('vehicles', 0) if isinstance(cap_data, dict) else cap_data

            if vehicles > 0:
                color = cmap(norm(vehicles))
                plt.plot(x_road, inverted_y, color=color, linewidth=2, zorder=2)
                plt.text(mid_x, inverted_mid_y, str(vehicles), color='black',
                         fontsize=8, ha='center', va='center', zorder=5,
                         bbox=dict(facecolor='white', edgecolor='none', alpha=0.5, boxstyle="round,pad=0.3"))
                total_vehicles_map += vehicles

    # 4. Poligono del area de estudio
    if area_polygon:
        x_poly, y_poly = list(zip(*area_polygon))
        x_poly = list(x_poly)
        y_poly = list(y_poly)
        plt.plot(x_poly + [x_poly[0]], y_poly + [y_poly[0]], color='red', linewidth=2, zorder=6)

    # 5. Ajustar vista
    all_x = [x for x, y in stations]
    all_y = [y for x, y in stations]
    margin = -70
    ax.set_xlim(min(all_x) - margin, max(all_x) + margin)
    ax.set_ylim(min(all_y) - margin, max(all_y) + margin)
    ax.invert_yaxis()
    ax.set_aspect('equal', 'box')

    # 6. Colorbar
    sm = cm.ScalarMappable(cmap=cmap, norm=norm)
    sm.set_array([])
    cbar = plt.colorbar(sm, ax=ax, fraction=0.046, pad=0.04)
    cbar.set_label('Number of vehicles')

    # 7. Total en esquina
    print(f"Total vehiculos en el mapa: {total_vehicles_map}")
    ax.text(0.02, 0.98, f'Total vehicles in map: {total_vehicles_map}',
            transform=ax.transAxes, fontsize=12, verticalalignment='top',
            bbox=dict(boxstyle="round,pad=0.3", edgecolor='black', facecolor='white'))

    plt.title(f"Capacity graph of the {escenario} vehicle/edge scenario")
    plt.xlabel("X-axis (m)")
    plt.ylabel("Y-axis (m)")
    plt.savefig(output_image, dpi=300)
    plt.close()
    print(f"Imagen guardada en {output_image}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--capacidad', required=True, help='capacidad_madrid.json existente')
    parser.add_argument('--partials-dir', required=True, help='Directorio raiz de cuadrantes_madrid')
    parser.add_argument('--output', required=True, help='JSON de salida')
    parser.add_argument('--output-image', required=True, help='Imagen PNG de salida')
    parser.add_argument('--netfile', required=True, help='Archivo de red SUMO (.net.xml)')
    parser.add_argument('--estaciones', required=True, help='Archivo de estaciones base')
    parser.add_argument('--geojson', required=True, help='GeoJSON del area de estudio (all.json)')
    parser.add_argument('--escenario', type=int, required=True, help='Escenario optimo (ej. 10)')
    args = parser.parse_args()

    # Cargar fichero de capacidad existente como base
    with open(args.capacidad, 'r') as f:
        capacidad = json.load(f)
    print(f"Tramos en capacidad_madrid.json: {len(capacidad)}")

    cuadrantes = ['down_left', 'down_center', 'down_right', 'up_left', 'up_center', 'up_right']

    # Recopilar todos los partials de todos los cuadrantes
    # escenario -> {edge_id -> entry}
    # Si un tramo aparece en varios cuadrantes en el mismo escenario,
    # se queda con el que tenga mas vehiculos
    all_partials_global = {}

    for cuadrante in cuadrantes:
        cuadrante_dir = os.path.join(args.partials_dir, cuadrante)
        if not os.path.exists(cuadrante_dir):
            print(f"Directorio no encontrado: {cuadrante_dir}")
            continue

        for i in range(1, args.escenario + 1):
            partials_file = os.path.join(cuadrante_dir, f'partials{i}.json')
            if not os.path.exists(partials_file):
                continue
            with open(partials_file, 'r') as f:
                try:
                    data = json.load(f)
                except Exception as e:
                    print(f"Error leyendo {partials_file}: {e}")
                    continue

            if i not in all_partials_global:
                all_partials_global[i] = {}

            for entry in data:
                edge_id = entry.get('edge', '')
                if not edge_id:
                    continue
                if edge_id not in all_partials_global[i] or \
                        entry.get('vehicles', 0) > all_partials_global[i][edge_id].get('vehicles', 0):
                    all_partials_global[i][edge_id] = entry

        print(f"Cuadrante {cuadrante}: cargado")

    # Obtener todos los tramos vistos en cualquier escenario
    all_edges = set()
    for i_data in all_partials_global.values():
        all_edges.update(i_data.keys())
    print(f"Total tramos unicos encontrados: {len(all_edges)}")

    # Para cada tramo, recorrer escenarios en orden y quedarse con el
    # ultimo valor NO saturado (cuando vea saturado, para)
    best_vehicles = {}
    edge_data_optimal = {}

    for edge_id in all_edges:
        last_good_vehicles = 0
        last_good_avg_rate = None

        for i in sorted(all_partials_global.keys()):
            entry = all_partials_global[i].get(edge_id)
            if entry is None:
                continue
            saturated = entry.get('saturated', True)
            vehicles = entry.get('vehicles', 0)

            if not saturated and vehicles > 0:
                last_good_vehicles = vehicles
                rates = [v['rate'] for v in entry.get('vehicle_rates', {}).values()]
                if rates:
                    last_good_avg_rate = sum(rates) / len(rates)
            elif saturated:
                break

        if last_good_vehicles > 0:
            best_vehicles[edge_id] = last_good_vehicles
            if last_good_avg_rate is not None:
                edge_data_optimal[edge_id] = {
                    'avg_rate': last_good_avg_rate,
                    'vehicles': last_good_vehicles
                }

    print(f"Tramos con datos no saturados encontrados: {len(best_vehicles)}")

    # Solo añadir tramos nuevos que no estaban en el JSON original
    # Los que ya existian se conservan sin modificar
    added = 0
    for edge_id, vehicles in best_vehicles.items():
        if edge_id not in capacidad:
            capacidad[edge_id] = {"vehicles": vehicles}
            added += 1

    print(f"Tramos nuevos añadidos: {added}")
    print(f"Tramos originales conservados: {len(capacidad) - added}")
    print(f"Total tramos en JSON final: {len(capacidad)}")

    with open(args.output, 'w') as f:
        json.dump(capacidad, f, indent=2)
    print(f"JSON guardado en: {args.output}")

    # Generar imagen
    print("Cargando red de SUMO...")
    net = sumolib.net.readNet(args.netfile)
    stations = read_stations(args.estaciones)

    with open(args.geojson, 'r') as f:
        geojson_data = json.load(f)
        geojson_coords = geojson_data['features'][0]['geometry']['coordinates'][0]

    generate_heatmap(capacidad, net, stations, geojson_coords, args.output_image, args.escenario)


if __name__ == '__main__':
    main()