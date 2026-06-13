#ifndef _CAPACITYSERVER_H_
#define _CAPACITYSERVER_H_

#include <list>
#include <queue>
#include <map>
#include <set>
#include <vector>
#include <string>

#include <omnetpp.h>

#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/common/L3AddressTag_m.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>

#include "apps/PLRut/CapacityPackets_m.h"

#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins_inet/VeinsInetMobility.h"

using veins::VeinsInetMobility;
using namespace std;
using namespace inet;
using namespace veins;

struct PendingReply {
    std::string carId;
    inet::L3Address destAddr;
    std::string newRouteEdges;
    bool hasNewRoute;
    bool wait;
};

struct WaitingCar {
    std::string     carId;
    inet::L3Address destAddr;
    std::string     fromEdge;
    std::string     toEdge;
    std::vector<std::string> originalRoute;
};

class CapacityServer : public omnetpp::cSimpleModule
{
    inet::UdpSocket socket;

    int iDtalk_;
    int size_;
    int localPort_;
    int destPort_;

    unsigned int totalSentBytes_;
    omnetpp::simsignal_t capacityGeneratedThroughtput_;

    omnetpp::cMessage *selfSender_;

    std::queue<PendingReply> replyQueue_;

    std::string mapName;
    std::string duarouterPath_;
    std::string tmpDir_;
    int defaultCapacity_;
    bool enableMaxDistance_;
    int maxDistance_;

    ~CapacityServer();

    typedef std::list<CapacityPacket*> PacketsList;
    PacketsList mPacketsList_;
    PacketsList mPlayoutQueue_;
    unsigned int mCurrentTalkspurt_;

    bool mInit_;
    unsigned int totalRcvdBytes_;

    void enqueueReply(const PendingReply& reply);
    void sendNextReply();

    std::set<std::string> getBlockedEdges();
    void loadCapacityJson(const std::string& path);
    std::vector<std::string> parseRoute(const std::string& routeStr);
    std::string joinRoute(const std::vector<std::string>& route);

    void reserveRoute(const std::string& carId, const std::vector<std::string>& route);
    void releaseRoute(const std::string& carId);

    // Libera edges ya recorridos por el coche (anteriores a currentEdge)
    void releasePassedEdges(const std::string& carId, const std::string& currentEdge);

    bool routeBlockedForCar(const std::string& carId, const std::vector<std::string>& route);
    int getCapacityForEdge(const std::string& edge);

    std::vector<std::string> computeAlternativeRoute(
        const std::string& fromEdge,
        const std::string& toEdge,
        const std::set<std::string>& blockedEdges,
        const std::string& carId);

    void writeTripsFile(const std::string& tripsFile,
                        const std::string& fromEdge,
                        const std::string& toEdge);

    void markEdgesAsBlocked(const std::string& filename,
                            const std::set<std::string>& blockedEdges);

    void retryWaitingCars();

public:
    CapacityServer();

protected:
    std::map<std::string, int>                       edgeCapacity_;
    std::map<std::string, int>                       edgeOccupation_;
    std::map<std::string, std::vector<std::string>>  reservedRoutesByCar_;
    std::map<std::string, std::string>               originalDestinationByCar_;
    std::list<WaitingCar>                            waitingCars_;

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage *msg) override;
};

#endif
