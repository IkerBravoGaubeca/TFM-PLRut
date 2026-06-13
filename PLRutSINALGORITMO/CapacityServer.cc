#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <omnetpp.h>
#include <nlohmann/json.hpp>

#include <inet/common/INETDefs.h>
#include <inet/common/packet/Packet.h>
#include <inet/networklayer/common/L3AddressTag_m.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>

#include "CapacityServer.h"

Define_Module(CapacityServer);

using namespace std;
using namespace inet;
using json = nlohmann::json;

CapacityServer::CapacityServer()
{
    totalRcvdBytes_ = 0;

    while (!mPacketsList_.empty()) {
        delete mPacketsList_.front();
        mPacketsList_.pop_front();
    }
}

CapacityServer::~CapacityServer()
{
    while (!mPacketsList_.empty()) {
        delete mPacketsList_.front();
        mPacketsList_.pop_front();
    }
}

void CapacityServer::initialize(int stage)
{
    EV << "[CapacityServer SIN ALGORITMO] initialize stage=" << stage << endl;

    cSimpleModule::initialize(stage);

    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    localPort_ = par("localPort");
    defaultCapacity_ = par("defaultCapacity");

    capacityFile_ = par("capacityFile").stringValue();
    violationsFile_ = par("violationsFile").stringValue();

    loadCapacityJson(capacityFile_);

    socket.setOutputGate(gate("socketOut"));
    socket.bind(localPort_);

    EV << "[CapacityServer SIN ALGORITMO] Ready. port=" << localPort_
       << " capacityFile=" << capacityFile_
       << " defaultCapacity=" << defaultCapacity_
       << " violationsFile=" << violationsFile_
       << endl;
}

void CapacityServer::handleMessage(omnetpp::cMessage *msg)
{
    Packet* pPacket = dynamic_cast<Packet*>(msg);

    if (!pPacket) {
        delete msg;
        return;
    }

    auto capacityHeader = pPacket->popAtFront<CapacityPacket>();

    std::string actualCarId = capacityHeader->getCarId();
    std::string currentEdge = capacityHeader->getCurrentEdge();
    bool arrived = capacityHeader->getArrived();
    int omnetIndex = capacityHeader->getOmnetIndex();

    mPacketsList_.push_back(capacityHeader->dup());

    delete pPacket;

    if (actualCarId.empty())
        return;

    if (omnetIndex >= 0 && omnetIndexByCar_.find(actualCarId) == omnetIndexByCar_.end()) {
        omnetIndexByCar_[actualCarId] = omnetIndex;

        EV << "[CapacityServer SIN ALGORITMO] Registered car="
           << actualCarId
           << " -> omnetIndex=" << omnetIndex
           << endl;
    }

    cleanupGoneCars();

    EV << "[CapacityServer SIN ALGORITMO] Received car=" << actualCarId
       << " currentEdge=" << currentEdge
       << " arrived=" << arrived
       << endl;

    if (arrived) {
        removeCarFromCurrentEdge(actualCarId);
        omnetIndexByCar_.erase(actualCarId);
        return;
    }

    if (currentEdge.empty())
        return;

    if (currentEdge[0] == ':') {
        EV << "[CapacityServer SIN ALGORITMO] Ignoring internal edge car="
           << actualCarId << " edge=" << currentEdge << endl;
        return;
    }

    updateOccupation(actualCarId, currentEdge);
}

omnetpp::cModule* CapacityServer::findCarModule(const std::string& carId)
{
    auto it = omnetIndexByCar_.find(carId);

    if (it == omnetIndexByCar_.end())
        return nullptr;

    std::string path = "Highway.car[" + std::to_string(it->second) + "]";
    return findModuleByPath(path.c_str());
}

void CapacityServer::cleanupGoneCars()
{
    std::vector<std::string> goneCars;

    for (const auto& kv : currentEdgeByCar_) {
        const std::string& carId = kv.first;

        omnetpp::cModule* mod = findCarModule(carId);

        if (mod == nullptr) {
            goneCars.push_back(carId);
        }
    }

    for (const auto& carId : goneCars) {
        EV << "[CapacityServer SIN ALGORITMO] Car "
           << carId
           << " module gone. Removing from occupation."
           << endl;

        removeCarFromCurrentEdge(carId);
        omnetIndexByCar_.erase(carId);
    }
}

void CapacityServer::updateOccupation(const std::string& carId,
                                      const std::string& currentEdge)
{
    std::string prevEdge = "";

    auto it = currentEdgeByCar_.find(carId);

    if (it != currentEdgeByCar_.end()) {
        prevEdge = it->second;
    }

    if (prevEdge == currentEdge)
        return;

    if (!prevEdge.empty()) {
        if (edgeOccupation_.count(prevEdge) && edgeOccupation_[prevEdge] > 0) {
            edgeOccupation_[prevEdge]--;
        }

        if (carsOnEdge_.count(prevEdge)) {
            carsOnEdge_[prevEdge].erase(carId);
        }

        EV << "[CapacityServer SIN ALGORITMO] car=" << carId
           << " leaves edge=" << prevEdge
           << " occ=" << edgeOccupation_[prevEdge]
           << "/" << getCapacityForEdge(prevEdge)
           << endl;
    }

    edgeOccupation_[currentEdge]++;
    carsOnEdge_[currentEdge].insert(carId);
    currentEdgeByCar_[carId] = currentEdge;

    EV << "[CapacityServer SIN ALGORITMO] car=" << carId
       << " enters edge=" << currentEdge
       << " occ=" << edgeOccupation_[currentEdge]
       << "/" << getCapacityForEdge(currentEdge)
       << endl;

    registerViolationIfNeeded(currentEdge, carId);
}

void CapacityServer::removeCarFromCurrentEdge(const std::string& carId)
{
    auto it = currentEdgeByCar_.find(carId);

    if (it == currentEdgeByCar_.end())
        return;

    std::string edge = it->second;

    if (!edge.empty()) {
        if (edgeOccupation_.count(edge) && edgeOccupation_[edge] > 0) {
            edgeOccupation_[edge]--;
        }

        if (carsOnEdge_.count(edge)) {
            carsOnEdge_[edge].erase(carId);
        }

        EV << "[CapacityServer SIN ALGORITMO] car=" << carId
           << " removed from edge=" << edge
           << " occ=" << edgeOccupation_[edge]
           << "/" << getCapacityForEdge(edge)
           << endl;
    }

    currentEdgeByCar_.erase(it);
}

void CapacityServer::registerViolationIfNeeded(const std::string& edge,
                                               const std::string& triggerCar)
{
    int cap = getCapacityForEdge(edge);
    int occ = edgeOccupation_[edge];

    if (occ <= cap)
        return;

    ViolationEvent event;
    event.time = simTime();
    event.edge = edge;
    event.capacity = cap;
    event.occupation = occ;
    event.triggerCar = triggerCar;
    event.carsOnEdge = carsOnEdge_[edge];

    violationEvents_.push_back(event);

    violatingVehicles_.insert(triggerCar);

    for (const auto& car : carsOnEdge_[edge]) {
        violatingVehicles_.insert(car);
    }

    EV << "[CapacityServer SIN ALGORITMO] VIOLATION "
       << "time=" << simTime()
       << " edge=" << edge
       << " cap=" << cap
       << " occ=" << occ
       << " triggerCar=" << triggerCar
       << " cars=";

    for (const auto& car : carsOnEdge_[edge]) {
        EV << car << " ";
    }

    EV << endl;
}

void CapacityServer::finish()
{
    if (violationsFile_.empty())
        return;

    std::ofstream file(violationsFile_);

    if (!file.is_open()) {
        EV << "[CapacityServer SIN ALGORITMO] Cannot open violations file: "
           << violationsFile_ << endl;
        return;
    }

    file << "Eventos de incumplimiento de capacidad\n";
    file << "================================================================================\n";
    file << "Tiempo | Edge | Capacidad | Ocupacion | Coche que provoca | Coches en el tramo\n";
    file << "--------------------------------------------------------------------------------\n";

    for (const auto& ev : violationEvents_) {
        file << ev.time << " | "
             << ev.edge << " | "
             << ev.capacity << " | "
             << ev.occupation << " | "
             << ev.triggerCar << " | ";

        bool first = true;

        for (const auto& car : ev.carsOnEdge) {
            if (!first)
                file << ",";

            file << car;
            first = false;
        }

        file << "\n";
    }

    file << "--------------------------------------------------------------------------------\n";
    file << "Total eventos de incumplimiento: " << violationEvents_.size() << "\n";
    file << "Total vehiculos distintos implicados: " << violatingVehicles_.size() << "\n";
    file << "Vehiculos implicados: ";

    bool first = true;

    for (const auto& carId : violatingVehicles_) {
        if (!first)
            file << ",";

        file << carId;
        first = false;
    }

    file << "\n";
    file << "================================================================================\n";

    file.close();

    EV << "[CapacityServer SIN ALGORITMO] Violations written to "
       << violationsFile_
       << ". Events=" << violationEvents_.size()
       << " vehicles=" << violatingVehicles_.size()
       << endl;
}

void CapacityServer::loadCapacityJson(const std::string& path)
{
    if (path.empty()) {
        EV << "[CapacityServer SIN ALGORITMO] Empty capacity file path." << endl;
        return;
    }

    std::ifstream file(path);

    if (!file.is_open()) {
        EV << "[CapacityServer SIN ALGORITMO] Cannot open capacity file: "
           << path << endl;
        return;
    }

    json data;
    file >> data;

    for (auto& [edgeId, info] : data.items()) {
        if (info.is_number_integer()) {
            edgeCapacity_[edgeId] = info.get<int>();
        }
        else if (info.is_object() && info.contains("vehicles")) {
            edgeCapacity_[edgeId] = info["vehicles"].get<int>();
        }
    }

    EV << "[CapacityServer SIN ALGORITMO] Loaded "
       << edgeCapacity_.size()
       << " edge capacities."
       << endl;
}

int CapacityServer::getCapacityForEdge(const std::string& edge)
{
    auto it = edgeCapacity_.find(edge);

    if (it != edgeCapacity_.end())
        return it->second;

    return defaultCapacity_;
}
