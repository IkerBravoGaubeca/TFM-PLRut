#ifndef _CAPACITYVEHICLE_H_
#define _CAPACITYVEHICLE_H_

#include <list>
#include <string>

#include <omnetpp.h>

#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include <inet/common/packet/Packet.h>
#include <inet/common/Ptr.h>
#include <inet/common/TagBase.h>

#include "apps/PLRut/CapacityPackets_m.h"

#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins_inet/VeinsInetMobility.h"

using veins::VeinsInetMobility;
using namespace std;
using namespace inet;
using namespace veins;

class CapacityVehicle : public omnetpp::cSimpleModule,
                        public inet::UdpSocket::ICallback
{
    inet::UdpSocket socket;

    omnetpp::simtime_t durTalk_;
    omnetpp::cMessage* selfSource_;
    omnetpp::cMessage* selfSender_;
    omnetpp::cMessage* initTraffic_;

    int iDtalk_;
    int size_;
    std::string navMessage_;

    unsigned int totalSentBytes_;
    omnetpp::simsignal_t capacityGeneratedThroughtput_;

    omnetpp::simtime_t timestamp_;
    omnetpp::simtime_t startTime_;

    int localPort_;
    int destPort_;
    inet::L3Address destAddress_;

    bool routeSent_;
    bool arrivedSent_;
    bool isStopped_;

    ~CapacityVehicle();

    typedef std::list<CapacityPacket*> PacketsList;
    PacketsList mPacketsList_;
    PacketsList mPlayoutQueue_;
    unsigned int mCurrentTalkspurt_;

    bool mInit_;
    unsigned int totalRcvdBytes_;

    void initTraffic();
    void selectPeriodTime();
    void sendCapacityPacket();
    void processReply(inet::Packet* pkt);

public:
    CapacityVehicle();

    virtual void socketDataArrived(inet::UdpSocket* socket,
                                   inet::Packet* packet) override;

    virtual void socketErrorArrived(inet::UdpSocket* socket,
                                    inet::Indication* indication) override {}

    virtual void socketClosed(inet::UdpSocket* socket) override {}

protected:
    VeinsInetMobility* mobility;
    TraCICommandInterface* traci;
    TraCICommandInterface::Vehicle* traciVehicle;

    std::string carId;
    int omnetIndex_;

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage *msg) override;
    void finish() override;
};

#endif
