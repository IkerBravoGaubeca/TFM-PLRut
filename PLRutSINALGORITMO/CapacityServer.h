#ifndef _CAPACITYSERVER_H_
#define _CAPACITYSERVER_H_

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <omnetpp.h>

#include <inet/common/packet/Packet.h>
#include <inet/networklayer/common/L3AddressTag_m.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>

#include "apps/PLRut/CapacityPackets_m.h"

using namespace std;
using namespace inet;

struct ViolationEvent {
    omnetpp::simtime_t time;
    std::string edge;
    int capacity;
    int occupation;
    std::string triggerCar;
    std::set<std::string> carsOnEdge;
};

class CapacityServer : public omnetpp::cSimpleModule
{
    inet::UdpSocket socket;

    int localPort_;
    int defaultCapacity_;

    std::string capacityFile_;
    std::string violationsFile_;

    typedef std::list<CapacityPacket*> PacketsList;
    PacketsList mPacketsList_;

    unsigned int totalRcvdBytes_;

    void loadCapacityJson(const std::string& path);
    int getCapacityForEdge(const std::string& edge);

    void updateOccupation(const std::string& carId,
                          const std::string& currentEdge);

    void removeCarFromCurrentEdge(const std::string& carId);

    void registerViolationIfNeeded(const std::string& edge,
                                   const std::string& triggerCar);

    omnetpp::cModule* findCarModule(const std::string& carId);
    void cleanupGoneCars();

public:
    CapacityServer();
    virtual ~CapacityServer();

protected:
    std::map<std::string, int> edgeCapacity_;
    std::map<std::string, int> edgeOccupation_;
    std::map<std::string, std::string> currentEdgeByCar_;
    std::map<std::string, std::set<std::string>> carsOnEdge_;

    std::map<std::string, int> omnetIndexByCar_;

    std::vector<ViolationEvent> violationEvents_;
    std::set<std::string> violatingVehicles_;

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
};

#endif
