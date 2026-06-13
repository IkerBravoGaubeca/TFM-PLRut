#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>

#include <inet/common/TimeTag_m.h>
#include "inet/common/INETDefs.h"
#include <omnetpp.h>

#include "CapacityVehicle.h"

Define_Module(CapacityVehicle);
using namespace std;
using namespace inet;

CapacityVehicle::CapacityVehicle()
{
    selfSource_  = nullptr;
    selfSender_  = nullptr;
    initTraffic_ = nullptr;

    while (!mPlayoutQueue_.empty()) {
        delete mPlayoutQueue_.front();
        mPlayoutQueue_.pop_front();
    }
    while (!mPacketsList_.empty()) {
        delete mPacketsList_.front();
        mPacketsList_.pop_front();
    }
}

CapacityVehicle::~CapacityVehicle()
{
    if (selfSource_)   cancelAndDelete(selfSource_);
    if (selfSender_)   cancelAndDelete(selfSender_);
    if (initTraffic_)  cancelAndDelete(initTraffic_);
}

void CapacityVehicle::initialize(int stage)
{
    EV << "[CapacityVehicle] initialize stage=" << stage << endl;
    cSimpleModule::initialize(stage);
    if (stage != inet::INITSTAGE_APPLICATION_LAYER) return;

    selfSource_  = new cMessage("selfSource");
    selfSender_  = new cMessage("selfSender");
    initTraffic_ = new cMessage("initTraffic");

    iDtalk_      = 0;
    timestamp_   = 0;
    size_        = par("PacketSize");
    localPort_   = par("localPort");
    destPort_    = par("destPort");
    navMessage_  = "";

    routeSent_   = false;
    arrivedSent_ = false;
    isStopped_   = false;

    totalSentBytes_               = 0;
    capacityGeneratedThroughtput_ = registerSignal("capacityGeneratedThroughput");

    mInit_          = true;
    totalRcvdBytes_ = 0;

    // Guardar tiempo de inicio
    startTime_ = simTime();

    mobility      = VeinsInetMobilityAccess().get(getParentModule());
    traci         = mobility->getCommandInterface();
    traciVehicle  = mobility->getVehicleCommandInterface();
    carId         = mobility->getExternalId();

    omnetIndex_ = getParentModule()->getIndex();

    EV << "[CapacityVehicle] carId=" << carId
       << " omnetIndex=" << omnetIndex_ << endl;

    initTraffic();
}

void CapacityVehicle::finish()
{
    // Escribir tiempo de viaje al fichero
    simtime_t travelTime = simTime() - startTime_;
    std::string outputFile = par("travelTimeFile").stringValue();
    if (!outputFile.empty()) {
        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << carId << ": " << travelTime.dbl() << "s\n";
            file.close();
        }
    }

    if (!arrivedSent_ && routeSent_ && !destAddress_.isUnspecified()) {
        EV << "[CapacityVehicle " << carId << "] finish() fallback - sending arrived." << endl;
        try {
            Packet* packet = new inet::Packet("capacity");
            auto capacityPkt = makeShared<CapacityPacket>();
            capacityPkt->setIDtalk(iDtalk_++);
            capacityPkt->setChunkLength(B(size_));
            capacityPkt->addTag<CreationTimeTag>()->setCreationTime(simTime());
            capacityPkt->setCarId(carId.c_str());
            capacityPkt->setOmnetIndex(omnetIndex_);
            capacityPkt->setArrived(true);
            capacityPkt->setHasNewRoute(false);
            capacityPkt->setWait(false);
            capacityPkt->setNewRouteEdges("");
            capacityPkt->setCurrentEdge("");
            capacityPkt->setDestinationEdge("");
            capacityPkt->setRouteEdges("");
            packet->insertAtBack(capacityPkt);
            socket.sendTo(packet, destAddress_, destPort_);
            arrivedSent_ = true;
        }
        catch (const std::exception& e) {
            EV << "[CapacityVehicle " << carId << "] ERROR in finish(): " << e.what() << endl;
        }
    }
}

void CapacityVehicle::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (!strcmp(msg->getName(), "selfSender"))
            sendCapacityPacket();
        else if (!strcmp(msg->getName(), "selfSource"))
            selectPeriodTime();
        else
            initTraffic();
        return;
    }

    Packet* pPacket = dynamic_cast<Packet*>(msg);
    if (pPacket != nullptr) {
        processReply(pPacket);
        return;
    }

    socket.processMessage(msg);
}

void CapacityVehicle::socketDataArrived(UdpSocket* /*socket*/, Packet* packet)
{
    processReply(packet);
}

void CapacityVehicle::processReply(Packet* pPacket)
{
    auto capacityHeader = pPacket->popAtFront<CapacityPacket>();

    if (mInit_) {
        mCurrentTalkspurt_ = capacityHeader->getIDtalk();
        mInit_ = false;
    }

    bool hasNewRoute = capacityHeader->getHasNewRoute();
    bool wait        = capacityHeader->getWait();

    if (hasNewRoute)
    {
        if (isStopped_) {
            try {
                traciVehicle->setSpeed(-1);
                isStopped_ = false;
                EV << "[CapacityVehicle " << carId << "] Speed resumed." << endl;
            }
            catch (const std::exception& e) {
                EV << "[CapacityVehicle " << carId << "] ERROR resuming: " << e.what() << endl;
            }
        }

        std::string strRoutes = capacityHeader->getNewRouteEdges();
        std::stringstream ss(strRoutes);
        std::list<std::string> routesList;
        std::string substr;
        while (std::getline(ss, substr, ','))
            if (!substr.empty())
                routesList.push_back(substr);

        EV << "[CapacityVehicle " << carId << "] NEW ROUTE received: " << strRoutes << endl;
        try {
            traciVehicle->changeVehicleRoute(routesList);
            EV << "[CapacityVehicle " << carId << "] Route changed OK." << endl;
        }
        catch (const std::exception& e) {
            EV << "[CapacityVehicle " << carId << "] ERROR changing route: " << e.what() << endl;
        }
    }
    else if (wait)
    {
        EV << "[CapacityVehicle " << carId << "] Server says WAIT. Stopping vehicle." << endl;
        try {
            traciVehicle->setSpeed(0);
            isStopped_ = true;
            EV << "[CapacityVehicle " << carId << "] Vehicle stopped." << endl;
        }
        catch (const std::exception& e) {
            EV << "[CapacityVehicle " << carId << "] ERROR stopping: " << e.what() << endl;
        }
    }
    else
    {
        if (isStopped_) {
            try {
                traciVehicle->setSpeed(-1);
                isStopped_ = false;
                EV << "[CapacityVehicle " << carId << "] Route free. Speed resumed." << endl;
            }
            catch (const std::exception& e) {
                EV << "[CapacityVehicle " << carId << "] ERROR resuming: " << e.what() << endl;
            }
        } else {
            EV << "[CapacityVehicle " << carId << "] Reply: route reserved OK." << endl;
        }
    }

    totalRcvdBytes_ += (int)B(capacityHeader->getChunkLength()).get();
    auto packetToBeQueued = capacityHeader->dup();
    mPacketsList_.push_back(packetToBeQueued);
    delete pPacket;
}

void CapacityVehicle::initTraffic()
{
    std::string destAddress = par("destAddress").stringValue();
    cModule* destModule = findModuleByPath(destAddress.c_str());

    if (destModule == nullptr)
    {
        EV << simTime() << " [CapacityVehicle " << carId
           << "] destination " << destAddress << " not found, retrying..." << endl;
        simtime_t offset = 0.01;
        if (initTraffic_ == nullptr)
            initTraffic_ = new cMessage("initTraffic");
        scheduleAt(simTime() + offset, initTraffic_);
    }
    else
    {
        if (initTraffic_ != nullptr) {
            delete initTraffic_;
            initTraffic_ = nullptr;
        }

        socket.setOutputGate(gate("socketOut"));
        socket.setCallback(this);
        socket.bind(localPort_);

        int tos = par("tos");
        if (tos != -1) socket.setTos(tos);

        EV << simTime() << " [CapacityVehicle " << carId
           << "] socket bound on port " << localPort_
           << ", sending to server port " << destPort_ << endl;

        scheduleAt(simTime(), selfSource_);
    }
}

void CapacityVehicle::selectPeriodTime()
{
    if (!selfSender_->isScheduled())
        scheduleAt(simTime(), selfSender_);

    durTalk_ = par("reportInterval");

    if (routeSent_ && !arrivedSent_ && !isStopped_) {
        scheduleAt(simTime() + 0.1, selfSource_);
    } else {
        scheduleAt(simTime() + durTalk_, selfSource_);
    }
}

void CapacityVehicle::sendCapacityPacket()
{
    if (destAddress_.isUnspecified())
        destAddress_ = L3AddressResolver().resolve(par("destAddress"));

    std::list<std::string> lista = traciVehicle->getPlannedRoadIds();
    std::string currentEdge     = traciVehicle->getRoadId();
    std::string destinationEdge = lista.empty() ? "" : lista.back();

    bool arrived = lista.size() <= 2;

    if (arrivedSent_) return;

    if (routeSent_ && !arrived && isStopped_) {
        EV << "[CapacityVehicle " << carId
           << "] Resending route to server (waiting for free slot)." << endl;
        routeSent_ = false;
    }

    std::string routeString;
    for (auto it = lista.begin(); it != lista.end(); ++it) {
        routeString += *it;
        if (std::next(it) != lista.end()) routeString += ",";
    }

    Packet* packet = new inet::Packet("capacity");
    auto capacityPkt = makeShared<CapacityPacket>();

    capacityPkt->setIDtalk(iDtalk_++);
    capacityPkt->setChunkLength(B(size_));
    capacityPkt->addTag<CreationTimeTag>()->setCreationTime(simTime());
    capacityPkt->setCarId(carId.c_str());
    capacityPkt->setOmnetIndex(omnetIndex_);
    capacityPkt->setCurrentEdge(currentEdge.c_str());
    capacityPkt->setDestinationEdge(destinationEdge.c_str());
    capacityPkt->setRouteEdges(routeString.c_str());
    capacityPkt->setArrived(arrived);
    capacityPkt->setHasNewRoute(false);
    capacityPkt->setWait(false);
    capacityPkt->setNewRouteEdges("");

    packet->insertAtBack(capacityPkt);

    if (arrived) {
        EV << "[CapacityVehicle " << carId << "] Last edges - releasing reservation." << endl;
        arrivedSent_ = true;
    } else if (!routeSent_) {
        EV << "[CapacityVehicle " << carId << "] SEND route"
           << " currentEdge=" << currentEdge
           << " dest=" << destinationEdge
           << " route=" << routeString << endl;
        routeSent_ = true;
    } else {
        EV << "[CapacityVehicle " << carId << "] UPDATE position"
           << " currentEdge=" << currentEdge << endl;
    }

    socket.sendTo(packet, destAddress_, destPort_);
    totalSentBytes_ += size_;
}
