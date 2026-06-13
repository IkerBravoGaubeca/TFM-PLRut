package simu5g.apps.PLRut;
import inet.applications.contract.IApp;

simple CapacityVehicle like IApp
{
    parameters:
        int localPort = default(6000);
        int destPort = default(3000);
        string destAddress;
        int PacketSize = default(100);
        int tos = default(-1);
        double reportInterval @unit(s) = default(1s);
        string travelTimeFile = default("results/travel_times.txt");
    gates:
        output socketOut;
        input socketIn;
}