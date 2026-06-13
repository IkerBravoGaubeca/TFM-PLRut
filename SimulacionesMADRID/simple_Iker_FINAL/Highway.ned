//
//                  Simu5G
//
// Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
//

package simu5g.simulations.NR.simple_Iker_FINAL;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.inet.AdhocHost;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.ethernet.Eth10G;

import simu5g.world.radio.LteChannelControl;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.Upf;
import simu5g.common.binder.Binder;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.cars.NRCar;

import org.car2x.veins.modules.obstacle.ObstacleControl;
import org.car2x.veins.subprojects.veins_inet.VeinsInetManager;
import org.car2x.veins.modules.world.annotations.AnnotationManager;
import org.car2x.veins.visualizer.roads.RoadsCanvasVisualizer;

network Highway
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area
        double playgroundSizeY @unit(m); // y size of the area
        double playgroundSizeZ @unit(m); // z size of the area
        
        double width = 10886.92;   // ancho en metros (opcional)
        double height = 11931.58;  // alto en metros (opcional)

    submodules:
        annotations: AnnotationManager {
            @display("p=2722.3198,4817.155");
        }
        roadsCanvasVisualizer: RoadsCanvasVisualizer {
            @display("p=248.14523,4825.046");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=248.14523,5624.625;is=s");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                config = xmldoc("demo.xml");  // existe en tu carpeta
                @display("p=762.63574,4821.9814");
        }

        // Veins / TraCI
        veinsManager: VeinsInetManager {
            @display("p=1119.8196,4821.9814;is=s");
        }

        // LTE/5G core
        channelControl: LteChannelControl {
            @display("p=1559.0591,4821.9814;is=s");
        }
        binder: Binder {
            @display("p=2012.779,4821.9814;is=s");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=2263.7732,4821.9814;is=s");
        }

        // Core IP
        server: StandardHost {
            @display("p=3124.74,2318.886;is=n;i=device/server");
        }
        router: Router {
            @display("p=2784.856,2313.404;i=device/smallrouter");
        }
        upf: Upf {
            @display("p=2472.382,2318.886");
        }
        // Vector de coches: Veins los crea dinámicamente
        car[0]: NRCar {
            @display("is=vs;p=2233.47,499.81747");
        }

        				gNodeB1: gNodeB {
		    @display("p=1117.25197,1769.87999;is=vs");
		}
		
		gNodeB2: gNodeB {
		    @display("p=2076.22565,1699.78216;is=vs");
		}
		
		gNodeB3: gNodeB {
		    @display("p=1141.54184,1914.38342;is=vs");
		}
		
		gNodeB4: gNodeB {
		    @display("p=1705.83099,2418.40345;is=vs");
		}
		
		gNodeB5: gNodeB {
		    @display("p=2112.43428,2488.21523;is=vs");
		}
		
		gNodeB6: gNodeB {
		    @display("p=1698.57956,2262.93643;is=vs");
		}
		
		gNodeB7: gNodeB {
		    @display("p=2145.12049,1567.11571;is=vs");
		}
		
		gNodeB8: gNodeB {
		    @display("p=2134.02206,1900.04903;is=vs");
		}
		
		gNodeB9: gNodeB {
		    @display("p=1256.82620,2348.23138;is=vs");
		}
		
		gNodeB10: gNodeB {
		    @display("p=2105.95985,2232.84880;is=vs");
		}
		
		gNodeB11: gNodeB {
		    @display("p=1012.18936,2179.76547;is=vs");
		}
		
		gNodeB12: gNodeB {
		    @display("p=1315.05959,1427.33711;is=vs");
		}
		
		gNodeB13: gNodeB {
		    @display("p=1923.21751,1742.97941;is=vs");
		}
		
		gNodeB14: gNodeB {
		    @display("p=1744.50078,1819.27226;is=vs");
		}
		
		gNodeB15: gNodeB {
		    @display("p=1440.06283,1705.85014;is=vs");
		}
		
		gNodeB16: gNodeB {
		    @display("p=1348.27495,1516.40749;is=vs");
		}
		
		gNodeB17: gNodeB {
		    @display("p=1633.01417,1973.79903;is=vs");
		}
		
		gNodeB18: gNodeB {
		    @display("p=1618.60409,1651.76454;is=vs");
		}
		
		gNodeB19: gNodeB {
		    @display("p=1409.23253,2382.74756;is=vs");
		}
		
		gNodeB20: gNodeB {
		    @display("p=1312.75925,1715.93726;is=vs");
		}
		
		gNodeB21: gNodeB {
		    @display("p=1759.44358,2074.70639;is=vs");
		}
		
		gNodeB22: gNodeB {
		    @display("p=1938.86649,1909.61318;is=vs");
		}
		
		gNodeB23: gNodeB {
		    @display("p=1571.88348,2195.32808;is=vs");
		}
		
		gNodeB24: gNodeB {
		    @display("p=1478.49810,2205.68721;is=vs");
		}
		
		gNodeB25: gNodeB {
		    @display("p=976.39516,2412.59356;is=vs");
		}
		
		gNodeB26: gNodeB {
		    @display("p=1014.85658,1846.76549;is=vs");
		}
		
		gNodeB27: gNodeB {
		    @display("p=1523.46240,1884.12421;is=vs");
		}
		
		gNodeB28: gNodeB {
		    @display("p=1760.84841,1897.10644;is=vs");
		}
		
		gNodeB29: gNodeB {
		    @display("p=1774.03656,2374.54034;is=vs");
		}
		
		gNodeB30: gNodeB {
		    @display("p=1477.52777,2327.78713;is=vs");
		}
		
		gNodeB31: gNodeB {
		    @display("p=1218.31610,1859.49313;is=vs");
		}
		
		gNodeB32: gNodeB {
		    @display("p=1667.55499,1896.36789;is=vs");
		}
		
		gNodeB33: gNodeB {
		    @display("p=1938.86649,1909.61318;is=vs");
		}
		
		gNodeB34: gNodeB {
		    @display("p=2108.40383,1922.04898;is=vs");
		}
		
		gNodeB35: gNodeB {
		    @display("p=971.44365,2035.42372;is=vs");
		}
		
		gNodeB36: gNodeB {
		    @display("p=997.95238,1711.82754;is=vs");
		}
		
		gNodeB37: gNodeB {
		    @display("p=988.02853,1557.82490;is=vs");
		}
		
		gNodeB38: gNodeB {
		    @display("p=1016.13972,1440.26246;is=vs");
		}
		
		gNodeB39: gNodeB {
		    @display("p=1094.44679,2505.68214;is=vs");
		}
		
		gNodeB40: gNodeB {
		    @display("p=1091.83821,2336.44135;is=vs");
		}
		
		gNodeB41: gNodeB {
		    @display("p=1107.58734,1600.95544;is=vs");
		}
		
		gNodeB42: gNodeB {
		    @display("p=1112.69807,2070.61438;is=vs");
		}
		
		gNodeB43: gNodeB {
		    @display("p=1177.32924,1484.52381;is=vs");
		}
		
		gNodeB44: gNodeB {
		    @display("p=1216.19928,2223.16743;is=vs");
		}
		
		gNodeB45: gNodeB {
		    @display("p=1238.29455,2502.17958;is=vs");
		}
		
		gNodeB46: gNodeB {
		    @display("p=1277.64235,2002.34061;is=vs");
		}
		
		gNodeB47: gNodeB {
		    @display("p=1329.28345,2140.45430;is=vs");
		}
		
		gNodeB48: gNodeB {
		    @display("p=1359.09607,1905.29605;is=vs");
		}
		
		gNodeB49: gNodeB {
		    @display("p=1445.90561,2028.89604;is=vs");
		}
		
		gNodeB50: gNodeB {
		    @display("p=1468.65819,2492.89651;is=vs");
		}
		
		gNodeB51: gNodeB {
		    @display("p=1449.72125,1442.09598;is=vs");
		}
		
		gNodeB52: gNodeB {
		    @display("p=1600.84063,1511.20979;is=vs");
		}
		
		gNodeB53: gNodeB {
		    @display("p=1622.80100,2494.99311;is=vs");
		}
		
		gNodeB54: gNodeB {
		    @display("p=1719.14256,1421.78650;is=vs");
		}
		
		gNodeB55: gNodeB {
		    @display("p=1758.70650,1598.42970;is=vs");
		}
		
		gNodeB56: gNodeB {
		    @display("p=1809.69351,2528.19325;is=vs");
		}
		
		gNodeB57: gNodeB {
		    @display("p=1804.63248,2207.44867;is=vs");
		}
		
		gNodeB58: gNodeB {
		    @display("p=1855.90722,1500.15754;is=vs");
		}
		
		gNodeB59: gNodeB {
		    @display("p=1927.97145,2055.48042;is=vs");
		}
		
		gNodeB60: gNodeB {
		    @display("p=1917.01089,2441.46872;is=vs");
		}
		
		gNodeB61: gNodeB {
		    @display("p=1948.26542,1599.66839;is=vs");
		}
		
		gNodeB62: gNodeB {
		    @display("p=2003.20145,1439.68874;is=vs");
		}
		
		gNodeB63: gNodeB {
		    @display("p=2058.38423,2076.07743;is=vs");
		}

    connections allowunconnected:
        // Core IP
        server.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> upf.filterGate;

        // UPF ↔ gNB (solo 3 enlaces)
        upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB1.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB2.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB3.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB4.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB5.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB6.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB7.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB8.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB9.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB10.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB11.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB12.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB13.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB14.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB15.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB16.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB17.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB18.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB19.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB20.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB21.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB22.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB23.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB24.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB25.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB26.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB27.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB28.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB29.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB30.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB31.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB32.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB33.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB34.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB35.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB36.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB37.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB38.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB39.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB40.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB41.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB42.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB43.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB44.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB45.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB46.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB47.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB48.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB49.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB50.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB51.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB52.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB53.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB54.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB55.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB56.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB57.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB58.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB59.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB60.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB61.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB62.ppp;
		upf.pppg++ <--> Eth10G { @display("ls=,0,da"); } <--> gNodeB63.ppp;

        // X2 si lo necesitas más adelante:
}
