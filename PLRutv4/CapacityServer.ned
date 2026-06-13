package simu5g.apps.PLRut;
import inet.applications.contract.IApp;

simple CapacityServer like IApp
{
    parameters:
        int localPort = default(3000);
        int destPort = default(6000);
        int PacketSize = default(100);
        string capacityFile = default("");
        string mapName;
        string duarouterPath = default("/usr/bin/duarouter");
        string tmpDir = default("tmp");
        int defaultCapacity = default(7);
        double retryInterval @unit(s) = default(5s);
    gates:
        output socketOut;
        input socketIn;
}