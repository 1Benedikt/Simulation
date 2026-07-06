#include "CentralizedTrafficController.h"

#include "../messages/ControlMessages_m.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <limits>
#include <queue>
#include <sstream>

using namespace omnetpp;

namespace sdnproject {

Define_Module(CentralizedTrafficController);

namespace {

std::string trim(const std::string& value)
{
    auto begin = value.begin();
    while (begin != value.end() && std::isspace(static_cast<unsigned char>(*begin)))
        ++begin;

    auto end = value.end();
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1))))
        --end;

    return std::string(begin, end);
}

} // namespace

void CentralizedTrafficController::initialize()
{
    monitoringInterval = par("monitoringInterval").doubleValue();
    minDecisionInterval = par("minDecisionInterval").doubleValue();
    lastDecisionTime = SimTime(-1);
    strategy = par("strategy").stdstringValue();
    congestionThreshold = par("congestionThreshold").doubleValue();

    if (strategy.empty())
        strategy = "baseline";

    buildTopologyGraph();
    loadDemands();
    applyBaselinePolicy();

    auto *timer = new cMessage("monitor");
    scheduleAt(simTime() + monitoringInterval, timer);
}

void CentralizedTrafficController::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        collectNetworkState();
        if (strategy == "dynamic")
            runDynamicDecision();
        scheduleAt(simTime() + monitoringInterval, msg);
        return;
    }

    processReport(msg);
}

void CentralizedTrafficController::finish()
{
    recordScalar("knownLinks", static_cast<long>(links.size()));
    recordScalar("receivedNetworkStateReports", receivedReports);
    recordScalar("sentControlDecisions", sentDecisions);
}

void CentralizedTrafficController::collectNetworkState()
{
    // TODO for students:
    // Collect or estimate state from INET statistics, signals, or helper data.
    // Useful state includes link utilization, queue length, packet drops,
    // measured delay, failed links, or server/application load.
}

void CentralizedTrafficController::processReport(cMessage *msg)
{
    auto *report = dynamic_cast<NetworkStateReport *>(msg);
    if (report == nullptr) {
        delete msg;
        return;
    }

    receivedReports++;

    LinkState& state = links[report->getLinkId()];
    state.utilization = report->getUtilization();
    state.queueDelay = report->getQueueLength() * 0.001;
    state.failed = report->getFailed();

    EV_INFO << "Controller received report from " << report->getReporter()
            << " link=" << report->getLinkId()
            << " utilization=" << report->getUtilization()
            << " queueLength=" << report->getQueueLength()
            << " failed=" << report->getFailed() << "\n";

    bool canSendDecision = lastDecisionTime < SIMTIME_ZERO || simTime() - lastDecisionTime >= minDecisionInterval;
    if (strategy == "dynamic" && canSendDecision && (state.failed || state.utilization >= congestionThreshold)) {
        if (!demands.empty()) {
            const auto& demand = demands.front();
            auto path = choosePath(demand.first, demand.second);
            sendControlDecision("reroute", demand.first + "->" + demand.second, path, 1);
        }
    }

    delete report;
}

void CentralizedTrafficController::applyBaselinePolicy()
{
    baselinePaths.clear();

    for (const auto& demand : demands) {
        const auto& src = demand.first;
        const auto& dst = demand.second;
        auto path = choosePath(src, dst);
        auto key = src + "->" + dst;

        baselinePaths[key] = path;
        recordScalar(("baselinePathLength:" + key).c_str(), static_cast<long>(path.size()));
        EV_INFO << "Baseline shortest path " << key << ": " << pathToString(path) << "\n";
    }
}

void CentralizedTrafficController::loadDemands()
{
    demands.clear();

    std::string fileName = par("demandFile").stdstringValue();
    if (!fileName.empty()) {
        std::string resolved = getEnvir()->resolveResourcePath(fileName.c_str());
        std::ifstream input(resolved);

        if (input.is_open()) {
            std::string line;
            while (std::getline(input, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#')
                    continue;

                std::stringstream parser(line);
                std::string src;
                std::string dst;
                if (!std::getline(parser, src, ',') || !std::getline(parser, dst, ','))
                    continue;

                src = trim(src);
                dst = trim(dst);
                if (src == "source" && dst == "destination")
                    continue;
                if (!src.empty() && !dst.empty())
                    demands.push_back({src, dst});
            }

            EV_INFO << "Loaded " << demands.size() << " demands from " << fileName << "\n";
        }
        else {
            EV_WARN << "Could not open demand file '" << fileName << "'; using default demands\n";
        }
    }

    if (demands.empty())
        demands = defaultDemands();
}

std::vector<std::pair<std::string, std::string>> CentralizedTrafficController::defaultDemands() const
{
    return {
        {"host[0]", "host[8]"},
        {"host[1]", "host[9]"},
        {"host[2]", "host[10]"},
        {"host[3]", "host[11]"},
        {"host[4]", "host[12]"},
        {"host[5]", "host[13]"},
        {"host[6]", "host[14]"},
        {"host[7]", "host[15]"}
    };
}

void CentralizedTrafficController::runDynamicDecision()
{
    // Example policy idea:
    // 1. identify overloaded links or servers
    // 2. select a less costly path/server for new or affected traffic
    // 3. apply the decision through the abstraction chosen for the assignment
    //
    // Keep hysteresis or a minimum decision interval to avoid oscillation.
}

void CentralizedTrafficController::sendControlDecision(const std::string& action, const std::string& target, const std::vector<std::string>& path, int priority)
{
    if (!gate("decisionOut")->isConnected())
        return;

    auto *decision = new ControlDecision("controlDecision");
    decision->setAction(action.c_str());
    decision->setTarget(target.c_str());
    decision->setSelectedPath(pathToString(path).c_str());
    decision->setPriority(priority);
    decision->setIssuedAt(simTime());

    sentDecisions++;
    lastDecisionTime = simTime();
    send(decision, "decisionOut");
}

void CentralizedTrafficController::buildTopologyGraph()
{
    topology.clear();

    addBidirectionalLink("core[0]", "aggregation[0]");
    addBidirectionalLink("core[0]", "aggregation[1]");
    addBidirectionalLink("core[0]", "aggregation[2]");
    addBidirectionalLink("core[0]", "aggregation[3]");
    addBidirectionalLink("core[1]", "aggregation[0]");
    addBidirectionalLink("core[1]", "aggregation[1]");
    addBidirectionalLink("core[1]", "aggregation[2]");
    addBidirectionalLink("core[1]", "aggregation[3]");

    addBidirectionalLink("aggregation[0]", "edge[0]");
    addBidirectionalLink("aggregation[0]", "edge[1]");
    addBidirectionalLink("aggregation[1]", "edge[0]");
    addBidirectionalLink("aggregation[1]", "edge[1]");
    addBidirectionalLink("aggregation[2]", "edge[2]");
    addBidirectionalLink("aggregation[2]", "edge[3]");
    addBidirectionalLink("aggregation[3]", "edge[2]");
    addBidirectionalLink("aggregation[3]", "edge[3]");

    for (int i = 0; i < 4; i++) {
        addBidirectionalLink("host[" + std::to_string(i) + "]", "edge[0]");
        addBidirectionalLink("host[" + std::to_string(i + 4) + "]", "edge[1]");
        addBidirectionalLink("host[" + std::to_string(i + 8) + "]", "edge[2]");
        addBidirectionalLink("host[" + std::to_string(i + 12) + "]", "edge[3]");
    }
}

void CentralizedTrafficController::addBidirectionalLink(const std::string& a, const std::string& b)
{
    topology[a].push_back(b);
    topology[b].push_back(a);

    links[a + "--" + b] = LinkState();
    links[b + "--" + a] = LinkState();
}

std::vector<std::string> CentralizedTrafficController::choosePath(const std::string& src, const std::string& dst) const
{
    std::queue<std::string> queue;
    std::map<std::string, std::string> previous;

    queue.push(src);
    previous[src] = "";

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        if (current == dst)
            break;

        auto it = topology.find(current);
        if (it == topology.end())
            continue;

        for (const auto& next : it->second) {
            if (previous.find(next) != previous.end())
                continue;
            previous[next] = current;
            queue.push(next);
        }
    }

    if (previous.find(dst) == previous.end())
        return {};

    std::vector<std::string> path;
    for (auto current = dst; !current.empty(); current = previous[current])
        path.push_back(current);
    std::reverse(path.begin(), path.end());
    return path;
}

std::string CentralizedTrafficController::chooseServer(const std::string& client, const std::vector<std::string>& candidates) const
{
    // TODO:
    // Replace with least-loaded, round-robin, latency-aware, or fairness-aware
    // server selection. This is the simplest actuation mechanism for a robust
    // bachelor-level load-balancing project.
    return candidates.empty() ? "" : candidates.front();
}

double CentralizedTrafficController::pathCost(const std::vector<std::string>& path) const
{
    double cost = 0.0;
    for (const auto& hop : path) {
        auto it = links.find(hop);
        if (it == links.end()) {
            cost += 1.0;
            continue;
        }
        if (it->second.failed)
            return std::numeric_limits<double>::infinity();
        cost += 1.0 + 10.0 * it->second.utilization + it->second.queueDelay;
    }
    return cost;
}

std::string CentralizedTrafficController::pathToString(const std::vector<std::string>& path) const
{
    if (path.empty())
        return "<no path>";

    std::ostringstream out;
    for (size_t i = 0; i < path.size(); i++) {
        if (i != 0)
            out << " -> ";
        out << path[i];
    }
    return out.str();
}

} // namespace sdnproject
