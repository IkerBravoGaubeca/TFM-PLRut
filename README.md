# TFM-PLRut

Repositorio de código asociado al Trabajo Fin de Máster sobre planificación dinámica de rutas basada en la capacidad de red para un servicio de conducción remota sobre 5G.

## Contenido

El repositorio incluye:

* `Capacity_MADRID`: código empleado para la obtención del fichero de capacidad del escenario de Madrid.
* `PLRutSINALGORITMO`: versión de referencia de PLRut sin aplicar el algoritmo de control de capacidad.
* `PLRutv1`: reserva completa de la ruta hasta que el vehículo finaliza el recorrido.
* `PLRutv2`: reserva completa inicial con liberación progresiva de los tramos recorridos.
* `PLRutv3`: reserva mediante una ventana móvil alrededor de la posición del vehículo.
* `PLRutv4`: reserva mediante una ventana orientada hacia delante.
* `SimulacionesMADRID`: archivos de código y configuración utilizados en el escenario de simulación de Madrid.

## Tecnologías utilizadas

* OMNeT++
* Simu5G
* Veins
* INET
* SUMO
* C++
* Python

## Autor

Iker Bravo Gaubeca

Trabajo Fin de Máster realizado en la Escuela Técnica Superior de Ingenieros de Telecomunicación de la Universidad Politécnica de Madrid.
