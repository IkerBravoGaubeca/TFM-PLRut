package simu5g.apps.PLRut;

import inet.applications.contract.IApp;

simple CapacityServer like IApp
{
    parameters:
        int localPort = default(3000);
        int PacketSize = default(100);

        string capacityFile = default("");
        int defaultCapacity = default(2);

        string violationsFile = default("results/violations_sin_algoritmo.txt");

    gates:
        output socketOut;
        input socketIn;
}