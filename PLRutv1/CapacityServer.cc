#include <cmath>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

#include <inet/common/TimeTag_m.h>
#include "inet/common/INETDefs.h"
#include <omnetpp.h>

#include "CapacityServer.h"
#include <nlohmann/json.hpp>

Define_Module(CapacityServer);
using namespace std;
using namespace inet;
using json = nlohmann::json;

CapacityServer::CapacityServer()
{
    selfSender_ = nullptr;
    while (!mPlayoutQueue_.empty()) { delete mPlayoutQueue_.front(); mPlayoutQueue_.pop_front(); }
    while (!mPacketsList_.empty())  { delete mPacketsList_.front();  mPacketsList_.pop_front();  }
}

CapacityServer::~CapacityServer()
{
    if (selfSender_) cancelAndDelete(selfSender_);
}

void CapacityServer::initialize(int stage)
{
    EV << "[CapacityServer] initialize stage=" << stage << endl;
    cSimpleModule::initialize(stage);
    if (stage != inet::INITSTAGE_APPLICATION_LAYER) return;

    selfSender_ = new cMessage("selfSender");
    iDtalk_    = 0;
    size_      = par("PacketSize");
    localPort_ = par("localPort");
    destPort_  = par("destPort");

    mapName = par("mapName").stringValue();
    if (mapName.find(".net.xml") == std::string::npos) mapName += ".net.xml";

    duarouterPath_   = par("duarouterPath").stringValue();
    tmpDir_          = par("tmpDir").stringValue();
    defaultCapacity_ = par("defaultCapacity");

    enableMaxDistance_ = false; maxDistance_ = 0;
    if (hasPar("enableMaxDistance")) enableMaxDistance_ = par("enableMaxDistance");
    if (hasPar("maxDistance"))       maxDistance_       = par("maxDistance");

    totalSentBytes_               = 0;
    capacityGeneratedThroughtput_ = registerSignal("capacityGeneratedThroughput");

    loadCapacityJson(par("capacityFile").stringValue());
    system(("mkdir -p \"" + tmpDir_ + "\"").c_str());

    socket.setOutputGate(gate("socketOut"));
    socket.bind(localPort_);

    mInit_ = true; totalRcvdBytes_ = 0;

    EV << "[CapacityServer] Ready. port=" << localPort_
       << " map=" << mapName << " defaultCapacity=" << defaultCapacity_ << endl;
}

void CapacityServer::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (!strcmp(msg->getName(), "selfSender")) sendNextReply();
        return;
    }

    Packet* pPacket = dynamic_cast<Packet*>(msg);
    if (!pPacket) { delete msg; return; }

    auto addrInd = pPacket->findTag<L3AddressInd>();
    if (!addrInd) { delete pPacket; return; }
    inet::L3Address senderAddr = addrInd->getSrcAddress();

    auto capacityHeader = pPacket->popAtFront<CapacityPacket>();
    totalRcvdBytes_ += (int)B(capacityHeader->getChunkLength()).get();
    mPacketsList_.push_back(capacityHeader->dup());

    std::string msgRoute        = capacityHeader->getRouteEdges();
    std::string currentEdge     = capacityHeader->getCurrentEdge();
    std::string destinationEdge = capacityHeader->getDestinationEdge();
    std::string actualCarId     = capacityHeader->getCarId();
    bool        arrived         = capacityHeader->getArrived();
    delete pPacket;

    EV << "[CapacityServer] Received from car=" << actualCarId
       << " addr=" << senderAddr
       << " currentEdge=" << currentEdge
       << " arrived=" << arrived
       << " route=" << msgRoute << endl;

    if (arrived) {
        EV << "[CapacityServer] Car " << actualCarId << " arrived. Releasing route." << endl;
        releaseRoute(actualCarId);
        originalDestinationByCar_.erase(actualCarId);
        retryWaitingCars();
        return;
    }

    bool isWaiting = false;
    for (const auto& wc : waitingCars_)
        if (wc.carId == actualCarId) { isWaiting = true; break; }

    if (isWaiting) {
        EV << "[CapacityServer] Retry from waiting car=" << actualCarId
           << ". Checking if route is free." << endl;

        std::vector<std::string> toRelease;
        for (const auto& kv : reservedRoutesByCar_) {
            if (kv.first == actualCarId) continue;
            cModule* mod = findModuleByPath(("Highway.car[" + kv.first + "]").c_str());
            if (mod == nullptr) {
                EV << "[CapacityServer] Car " << kv.first
                   << " module gone. Releasing route." << endl;
                toRelease.push_back(kv.first);
            }
        }
        for (const auto& id : toRelease) {
            releaseRoute(id);
            originalDestinationByCar_.erase(id);
        }

        releaseRoute(actualCarId);

        std::vector<std::string> currentRoute = parseRoute(msgRoute);
        if (currentRoute.empty()) return;

        std::string from = currentEdge;
        if (!from.empty() && from[0] == ':') {
            for (const auto& e : currentRoute)
                if (!e.empty() && e[0] != ':') { from = e; break; }
        }

        for (auto& wc : waitingCars_) {
            if (wc.carId == actualCarId) {
                wc.fromEdge      = from;
                wc.originalRoute = currentRoute;
                break;
            }
        }

        PendingReply reply;
        reply.carId    = actualCarId;
        reply.destAddr = senderAddr;

        if (!routeBlockedForCar(actualCarId, currentRoute)) {
            EV << "[CapacityServer] Route now free for waiting car=" << actualCarId << endl;
            reserveRoute(actualCarId, currentRoute);
            waitingCars_.remove_if([&](const WaitingCar& wc) {
                return wc.carId == actualCarId;
            });
            reply.hasNewRoute   = false;
            reply.wait          = false;
            reply.newRouteEdges = "";
            enqueueReply(reply);
        } else {
            std::set<std::string> blocked = getBlockedEdges();
            std::string to = destinationEdge.empty() ?
                originalDestinationByCar_[actualCarId] : destinationEdge;

            std::vector<std::string> newRoute =
                computeAlternativeRoute(from, to, blocked, actualCarId);

            if (!newRoute.empty() && !routeBlockedForCar(actualCarId, newRoute)) {
                EV << "[CapacityServer] Alt route for waiting car=" << actualCarId
                   << ": " << joinRoute(newRoute) << endl;
                reserveRoute(actualCarId, newRoute);
                waitingCars_.remove_if([&](const WaitingCar& wc) {
                    return wc.carId == actualCarId;
                });
                reply.hasNewRoute   = true;
                reply.wait          = false;
                reply.newRouteEdges = joinRoute(newRoute);
                enqueueReply(reply);
            } else {
                EV << "[CapacityServer] Still blocked for waiting car=" << actualCarId
                   << ". Not reserving anything." << endl;
            }
        }
        return;
    }

    if (reservedRoutesByCar_.count(actualCarId)) return;

    std::vector<std::string> currentRoute = parseRoute(msgRoute);
    if (currentRoute.empty() || currentEdge.empty() || destinationEdge.empty()) {
        EV << "[CapacityServer] Incomplete data from car=" << actualCarId << ". Ignoring." << endl;
        return;
    }

    EV << "[CapacityServer] New car: " << actualCarId
       << " destination=" << destinationEdge << endl;
    originalDestinationByCar_[actualCarId] = destinationEdge;

    std::string from = currentEdge;
    if (!from.empty() && from[0] == ':') {
        for (const auto& e : currentRoute)
            if (!e.empty() && e[0] != ':') { from = e; break; }
    }
    std::string to = destinationEdge;

    if (!routeBlockedForCar(actualCarId, currentRoute)) {
        EV << "[CapacityServer] Initial route OK for " << actualCarId << endl;
        reserveRoute(actualCarId, currentRoute);
        return;
    }

    EV << "[CapacityServer] Route blocked for new car=" << actualCarId
       << ". Computing alternative from=" << from << " to=" << to << endl;

    std::set<std::string> blocked = getBlockedEdges();
    std::vector<std::string> newRoute = computeAlternativeRoute(from, to, blocked, actualCarId);

    PendingReply reply;
    reply.carId    = actualCarId;
    reply.destAddr = senderAddr;

    if (!newRoute.empty() && !routeBlockedForCar(actualCarId, newRoute)) {
        EV << "[CapacityServer] Alt route for " << actualCarId
           << ": " << joinRoute(newRoute) << endl;
        reserveRoute(actualCarId, newRoute);
        reply.hasNewRoute   = true;
        reply.wait          = false;
        reply.newRouteEdges = joinRoute(newRoute);
        enqueueReply(reply);
    } else {
        EV << "[CapacityServer] No alternative for car=" << actualCarId
           << ". Adding to waiting list WITHOUT reserving." << endl;

        WaitingCar wc;
        wc.carId         = actualCarId;
        wc.destAddr      = senderAddr;
        wc.fromEdge      = from;
        wc.toEdge        = to;
        wc.originalRoute = currentRoute;
        waitingCars_.push_back(wc);


        reply.hasNewRoute   = false;
        reply.wait          = true;
        reply.newRouteEdges = "";
        enqueueReply(reply);
    }
}

void CapacityServer::retryWaitingCars()
{
    if (waitingCars_.empty()) return;

    EV << "[CapacityServer] Retrying " << waitingCars_.size() << " waiting cars." << endl;

    std::set<std::string> blocked = getBlockedEdges();

    std::list<WaitingCar> stillWaiting;

    for (auto& wc : waitingCars_) {

        releaseRoute(wc.carId);

        if (!routeBlockedForCar(wc.carId, wc.originalRoute)) {
            EV << "[CapacityServer] Original route now free for car=" << wc.carId << endl;
            reserveRoute(wc.carId, wc.originalRoute);

            PendingReply reply;
            reply.carId         = wc.carId;
            reply.destAddr      = wc.destAddr;
            reply.hasNewRoute   = false;
            reply.wait          = false;
            reply.newRouteEdges = "";
            enqueueReply(reply);
            continue;
        }

        std::vector<std::string> newRoute =
            computeAlternativeRoute(wc.fromEdge, wc.toEdge, blocked, wc.carId);

        if (!newRoute.empty() && !routeBlockedForCar(wc.carId, newRoute)) {
            EV << "[CapacityServer] Alt route found for waiting car=" << wc.carId
               << ": " << joinRoute(newRoute) << endl;
            reserveRoute(wc.carId, newRoute);

            PendingReply reply;
            reply.carId         = wc.carId;
            reply.destAddr      = wc.destAddr;
            reply.hasNewRoute   = true;
            reply.wait          = false;
            reply.newRouteEdges = joinRoute(newRoute);
            enqueueReply(reply);
        } else {
            EV << "[CapacityServer] Still no route for waiting car=" << wc.carId
               << ". Keeping in waiting list without reservation." << endl;
            stillWaiting.push_back(wc);
        }
    }

    waitingCars_ = stillWaiting;
}

void CapacityServer::enqueueReply(const PendingReply& reply)
{
    replyQueue_.push(reply);
    if (!selfSender_->isScheduled())
        scheduleAt(simTime(), selfSender_);
}

void CapacityServer::sendNextReply()
{
    if (replyQueue_.empty()) return;

    PendingReply reply = replyQueue_.front();
    replyQueue_.pop();

    Packet* packet = new inet::Packet("capacityReply");
    auto header = makeShared<CapacityPacket>();
    header->setIDtalk(iDtalk_++);
    header->setChunkLength(B(size_));
    header->addTag<CreationTimeTag>()->setCreationTime(simTime());
    header->setCarId(reply.carId.c_str());
    header->setHasNewRoute(reply.hasNewRoute);
    header->setWait(reply.wait);
    header->setNewRouteEdges(reply.newRouteEdges.c_str());
    header->setCurrentEdge("");
    header->setDestinationEdge("");
    header->setRouteEdges("");
    header->setArrived(false);
    packet->insertAtBack(header);

    EV << "[CapacityServer] Reply -> car=" << reply.carId
       << " addr=" << reply.destAddr
       << " hasNewRoute=" << reply.hasNewRoute
       << " wait=" << reply.wait
       << " newRoute=" << reply.newRouteEdges << endl;

    socket.sendTo(packet, reply.destAddr, destPort_);
    totalSentBytes_ += size_;

    if (!replyQueue_.empty())
        scheduleAt(simTime(), selfSender_);
}

std::set<std::string> CapacityServer::getBlockedEdges()
{
    std::set<std::string> blocked;
    for (const auto& kv : edgeOccupation_)
        if (kv.second >= getCapacityForEdge(kv.first))
            blocked.insert(kv.first);
    return blocked;
}

void CapacityServer::loadCapacityJson(const std::string& path)
{
    if (path.empty()) { EV << "[CapacityServer] No capacity file. Using default=" << defaultCapacity_ << endl; return; }
    std::ifstream file(path);
    if (!file.is_open()) { EV << "[CapacityServer] Cannot open: " << path << endl; return; }
    json data; file >> data;
    for (auto& [tramo, info] : data.items()) {
        if (info.is_number_integer())
            edgeCapacity_[tramo] = info.get<int>();
        else if (info.is_object() && info.contains("vehicles"))
            edgeCapacity_[tramo] = info["vehicles"].get<int>();
    }
    EV << "[CapacityServer] Loaded " << edgeCapacity_.size() << " edge capacities." << endl;
}

int CapacityServer::getCapacityForEdge(const std::string& edge)
{
    auto it = edgeCapacity_.find(edge);
    return (it != edgeCapacity_.end()) ? it->second : defaultCapacity_;
}

std::vector<std::string> CapacityServer::parseRoute(const std::string& routeStr)
{
    std::vector<std::string> route;
    std::stringstream ss(routeStr);
    std::string token;
    while (std::getline(ss, token, ','))
        if (!token.empty()) route.push_back(token);
    return route;
}

std::string CapacityServer::joinRoute(const std::vector<std::string>& route)
{
    std::ostringstream oss;
    for (size_t i = 0; i < route.size(); ++i) { if (i > 0) oss << ","; oss << route[i]; }
    return oss.str();
}

void CapacityServer::releaseRoute(const std::string& carId)
{
    auto it = reservedRoutesByCar_.find(carId);
    if (it == reservedRoutesByCar_.end()) return;
    for (const auto& edge : it->second) {
        if (edgeOccupation_.count(edge) && edgeOccupation_[edge] > 0) {
            edgeOccupation_[edge]--;
            EV << "[CapacityServer] release car=" << carId
               << " edge=" << edge << " occ=" << edgeOccupation_[edge]
               << "/" << getCapacityForEdge(edge) << endl;
        }
    }
    reservedRoutesByCar_.erase(it);
}

void CapacityServer::reserveRoute(const std::string& carId,
                                  const std::vector<std::string>& route)
{
    releaseRoute(carId);
    for (const auto& edge : route) {
        edgeOccupation_[edge]++;
        EV << "[CapacityServer] reserve car=" << carId
           << " edge=" << edge << " occ=" << edgeOccupation_[edge]
           << "/" << getCapacityForEdge(edge) << endl;
    }
    reservedRoutesByCar_[carId] = route;
}

bool CapacityServer::routeBlockedForCar(const std::string& carId,
                                        const std::vector<std::string>& route)
{
    for (const auto& edge : route) {
        int cap = getCapacityForEdge(edge);
        int occ = edgeOccupation_.count(edge) ? edgeOccupation_[edge] : 0;
        if (reservedRoutesByCar_.count(carId)) {
            const auto& own = reservedRoutesByCar_[carId];
            if (std::find(own.begin(), own.end(), edge) != own.end()) occ--;
        }
        if (occ >= cap) {
            EV << "[CapacityServer] BLOCKED edge=" << edge
               << " occ=" << occ << "/" << cap << " for car=" << carId << endl;
            return true;
        }
    }
    return false;
}

void CapacityServer::writeTripsFile(const std::string& tripsFile,
                                    const std::string& fromEdge,
                                    const std::string& toEdge)
{
    std::ofstream archivo(tripsFile);
    if (!archivo.is_open()) return;
    archivo << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<routes>\n"
            << "    <trip id=\"0\" depart=\"0.00\" from=\"" << fromEdge
            << "\" to=\"" << toEdge << "\"/>\n</routes>\n";
    archivo.close();
    EV << "[CapacityServer] Trips file: " << tripsFile
       << " from=" << fromEdge << " to=" << toEdge << endl;
}

void CapacityServer::markEdgesAsBlocked(const std::string& filename,
                                        const std::set<std::string>& blockedEdges)
{
    std::ifstream inFile(filename);
    std::string tmpFile = filename + ".tmp";
    std::ofstream outFile(tmpFile);
    if (!inFile.is_open() || !outFile.is_open()) return;

    std::string line;
    bool insideBlockedEdge = false;
    while (std::getline(inFile, line)) {
        bool skipLine = false;
        for (const auto& edgeId : blockedEdges) {
            if (line.find("<edge id=\"" + edgeId + "\"") != std::string::npos) {
                outFile << line << "\n"; insideBlockedEdge = true; skipLine = true; break;
            }
        }
        if (insideBlockedEdge && line.find("</edge>") != std::string::npos) {
            outFile << line << "\n"; insideBlockedEdge = false; skipLine = true;
        }
        if (!skipLine) {
            if (!insideBlockedEdge || line.find("<lane") == std::string::npos) {
                outFile << line << "\n";
            } else {
                if (std::regex_search(line, std::regex("disallow=\"([^\"]*)\"")))
                    line = std::regex_replace(line, std::regex("disallow=\"([^\"]*)\""), "");
                if (std::regex_search(line, std::regex("allow=\"([^\"]*)\"")))
                    line = std::regex_replace(line, std::regex("allow=\"([^\"]*)\""), "allow=\"pedestrian\"");
                else {
                    size_t pos = line.find("/>");
                    if (pos != std::string::npos) line.insert(pos, " allow=\"pedestrian\"");
                }
                outFile << line << "\n";
            }
        }
    }
    inFile.close(); outFile.close();
    std::remove(filename.c_str());
    std::rename(tmpFile.c_str(), filename.c_str());
}

std::vector<std::string> CapacityServer::computeAlternativeRoute(
    const std::string& fromEdge,
    const std::string& toEdge,
    const std::set<std::string>& blockedEdges,
    const std::string& carId)
{
    std::vector<std::string> empty;
    if (fromEdge.empty() || toEdge.empty() || fromEdge == toEdge) return empty;

    std::string tripsFile = tmpDir_ + "/tmp_" + carId + ".trips.xml";
    std::string netFile   = tmpDir_ + "/tmp_" + carId + ".net.xml";
    std::string routeFile = tmpDir_ + "/tmp_" + carId + ".rou.xml";
    std::string logFile   = tmpDir_ + "/tmp_" + carId + ".log";

    std::set<std::string> filtered = blockedEdges;
    filtered.erase(fromEdge);
    filtered.erase(toEdge);

    writeTripsFile(tripsFile, fromEdge, toEdge);

    if (system(("cp \"" + mapName + "\" \"" + netFile + "\"").c_str()) != 0) return empty;
    if (!filtered.empty()) {
        EV << "[CapacityServer] Blocking " << filtered.size() << " edges for car=" << carId << endl;
        markEdgesAsBlocked(netFile, filtered);
    }

    std::string cmd = duarouterPath_
        + " -n \"" + netFile + "\""
        + " --route-files \"" + tripsFile + "\""
        + " -o \"" + routeFile + "\""
        + " --ignore-errors"
        + " > \"" + logFile + "\" 2>&1";

    EV << "[CapacityServer] Running duarouter: " << cmd << endl;
    if (system(cmd.c_str()) != 0) return empty;

    std::ifstream file(routeFile);
    if (!file.is_open()) return empty;

    std::ostringstream oss; oss << file.rdbuf();
    std::string xml = oss.str();

    std::string edges;
    std::smatch match;
    if (std::regex_search(xml, match, std::regex("edges=\"([^\"]+)\"")))
        edges = match[1].str();

    if (edges.empty()) { EV << "[CapacityServer] No edges found for car=" << carId << endl; return empty; }

    std::replace(edges.begin(), edges.end(), ' ', ',');
    EV << "[CapacityServer] Alternative route for car=" << carId << ": " << edges << endl;
    return parseRoute(edges);
}
