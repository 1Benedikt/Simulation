#include "CentralizedTrafficController.h"

#include "../messages/ControlMessages_m.h"
#include "PathEnumerator.h"

#include <algorithm>
#include <random>
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
    shufflePaths = par("shufflePaths").boolValue();
    congestionThreshold = par("congestionThreshold").doubleValue();
    overloadThreshold = par("overloadThreshold").intValue();
    timeToFullPathCoverage = SimTime(-1);

    congestedLinkCountSignal = registerSignal("congestedLinkCount");
    averageUtilizationSignal = registerSignal("averageUtilization");
    failedLinksCountSignal = registerSignal("failedLinksCount");
    fairnessIndexSignal = registerSignal("fairnessIndex");
    maxCoreLoadSignal = registerSignal("maxCoreLoad");
    maxAggregationLoadSignal = registerSignal("maxAggregationLoad");

    static const std::vector<std::pair<std::string, const char*>> routerSignalDefs = {
        {"core[0]",        "routerLoadCore0"},
        {"core[1]",        "routerLoadCore1"},
        {"aggregation[0]", "routerLoadAggregation0"},
        {"aggregation[1]", "routerLoadAggregation1"},
        {"aggregation[2]", "routerLoadAggregation2"},
        {"aggregation[3]", "routerLoadAggregation3"},
        {"edge[0]",        "routerLoadEdge0"},
        {"edge[1]",        "routerLoadEdge1"},
        {"edge[2]",        "routerLoadEdge2"},
        {"edge[3]",        "routerLoadEdge3"},
    };
    for (const auto& def : routerSignalDefs)
        routerLoadSignals[def.first] = registerSignal(def.second);

    if (strategy.empty())
        strategy = "baseline";


    buildTopologyGraph();
    loadDemands();
    initRoundRobin();
    applyBaselinePolicy();

    PathEnumerator enumerator(topology);
    for (const auto& demand : demands) {
        auto key = demand.first + "->" + demand.second;
        allPaths[key] = enumerator.enumerate(demand.first, demand.second);
    }

    if (shufflePaths)
        shuffleAllPaths();

    logAllPaths();

    auto *timer = new cMessage("monitor");
    scheduleAt(simTime() + monitoringInterval, timer);
}

void CentralizedTrafficController::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        monitoringTicks++;
        collectNetworkState();
        if (strategy == "dynamic")
            runDynamicDecision();
        emitRouterLoad();
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
    recordScalar("pathSwitchCount", pathSwitchCount);

    double totalDistinct = 0;
    for (const auto& entry : distinctPathsUsed) {
        recordScalar(("distinctPathsUsed:" + entry.first).c_str(),
                     static_cast<long>(entry.second.size()));
        totalDistinct += entry.second.size();
    }
    if (!distinctPathsUsed.empty())
        recordScalar("averageDistinctPathsPerDemand",
                     totalDistinct / static_cast<double>(distinctPathsUsed.size()));

    if (strategy == "baseline") {
        for (const auto& entry : baselinePaths)
            for (const auto& node : entry.second)
                cumulativeFanout[node] += monitoringTicks;
    }

    long denominator = static_cast<long>(demands.size()) * monitoringTicks;
    std::vector<std::string> coreAndAgg = {
        "core[0]", "core[1]",
        "aggregation[0]", "aggregation[1]", "aggregation[2]", "aggregation[3]"
    };
    for (const auto& node : coreAndAgg) {
        recordScalar(("fanout:" + node).c_str(), cumulativeFanout[node]);
        if (denominator > 0)
            recordScalar(("fanoutRate:" + node).c_str(),
                         cumulativeFanout[node] / static_cast<double>(denominator));
    }

    recordScalar("overloadedIntervals", overloadedIntervals);
    if (monitoringTicks > 0)
        recordScalar("overloadFrequency",
                     overloadedIntervals / static_cast<double>(monitoringTicks));

    // Jain's index over the cumulative fanout: long-run fairness, as opposed to
    // the per-interval fairnessIndex signal (instantaneous fairness).
    double sumX = 0.0, sumX2 = 0.0;
    for (const auto& node : coreAndAgg) {
        double x = static_cast<double>(cumulativeFanout[node]);
        sumX  += x;
        sumX2 += x * x;
    }
    double cumulativeFairness = (sumX2 == 0.0) ? 1.0 : (sumX * sumX) / (coreAndAgg.size() * sumX2);
    recordScalar("cumulativeFairnessIndex", cumulativeFairness);

    if (timeToFullPathCoverage >= SIMTIME_ZERO)
        recordScalar("timeToFullPathCoverage", timeToFullPathCoverage.dbl());
}

void CentralizedTrafficController::collectNetworkState()
{
    // TODO for students:
    // Collect or estimate state from INET statistics, signals, or helper data.
    // Useful state includes link utilization, queue length, packet drops,
    // measured delay, failed links, or server/application load.

    long congestedLinks = 0;
    double totalUtilization = 0.0;
    long consideredLinks = 0;
    long failedLinks = 0;

    for (const auto& entry : links) {
        const LinkState& state = entry.second;
         // NOTE: skip/guard against the phantom "a--b" entries that never
             // get updated by processReport() (key-mismatch issue flagged earlier)
             // -- otherwise this average is diluted by dead zero-utilization entries.
        totalUtilization += state.utilization;
        consideredLinks++;
        if (!state.failed && state.utilization >= congestionThreshold)
            congestedLinks++;

        if (state.failed)
            failedLinks++;
    }

    double averageUtilization = consideredLinks == 0 ? 0.0 : totalUtilization / consideredLinks;

    emit(congestedLinkCountSignal, congestedLinks);
    emit(averageUtilizationSignal, averageUtilization);
    emit(failedLinksCountSignal, failedLinks);

    EV_INFO << "Network state: " << consideredLinks << " known links, "
            << congestedLinks << " congested, average utilization="
            << averageUtilization << "\n";
}

void CentralizedTrafficController::emitRouterLoad()
{
    std::map<std::string, long> load;
    for (const auto& sig : routerLoadSignals)
        load[sig.first] = 0;

    const auto& activePaths = (strategy == "dynamic") ? currentPaths : baselinePaths;
    for (const auto& entry : activePaths)
        for (const auto& node : entry.second) {
            auto it = load.find(node);
            if (it != load.end())
                it->second++;
        }

    for (const auto& sig : routerLoadSignals)
        emit(sig.second, load[sig.first]);

    static const std::vector<std::string> fairnessNodes = {
        "core[0]", "core[1]",
        "aggregation[0]", "aggregation[1]", "aggregation[2]", "aggregation[3]"
    };

    double sumX = 0.0, sumX2 = 0.0;
    for (const auto& node : fairnessNodes) {
        double x = static_cast<double>(load[node]);
        sumX  += x;
        sumX2 += x * x;
    }
    double fairness = (sumX2 == 0.0) ? 1.0 : (sumX * sumX) / (fairnessNodes.size() * sumX2);
    emit(fairnessIndexSignal, fairness);

    long maxCoreLoad = std::max(load["core[0]"], load["core[1]"]);
    long maxAggregationLoad = 0;
    for (int i = 0; i < 4; i++)
        maxAggregationLoad = std::max(maxAggregationLoad,
                                      load["aggregation[" + std::to_string(i) + "]"]);
    emit(maxCoreLoadSignal, maxCoreLoad);
    emit(maxAggregationLoadSignal, maxAggregationLoad);

    if (std::max(maxCoreLoad, maxAggregationLoad) > overloadThreshold)
        overloadedIntervals++;
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
       // EV_INFO << "Baseline shortest path " << allPahts[key] << ": " << pathToString(path) << "\n";
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

    for (const auto& demand : demands) {
        std::string SrcDestinationKey = demand.first + "->" + demand.second;
        Node host = demand.first;
        std::vector<Path> availablePaths = allPaths[SrcDestinationKey];
        Path chosenPath = chooseServer(host, availablePaths);


        sendControlDecision("reroute", SrcDestinationKey, chosenPath, 1);

        if (currentPaths[SrcDestinationKey] != chosenPath) {
            currentPaths[SrcDestinationKey] = chosenPath;
            distinctPathsUsed[SrcDestinationKey].insert(pathToString(chosenPath));
            pathSwitchCount++;
        }

        for (const auto& node : chosenPath)
            cumulativeFanout[node]++;
    }

    if (timeToFullPathCoverage < SIMTIME_ZERO) {
        bool allCovered = true;
        for (const auto& demand : demands) {
            auto key = demand.first + "->" + demand.second;
            auto pathsIt = allPaths.find(key);
            auto usedIt = distinctPathsUsed.find(key);
            if (pathsIt == allPaths.end() || pathsIt->second.empty()
                    || usedIt == distinctPathsUsed.end()
                    || usedIt->second.size() < pathsIt->second.size()) {
                allCovered = false;
                break;
            }
        }
        if (allCovered)
            timeToFullPathCoverage = simTime();
    }
}

void CentralizedTrafficController::sendControlDecision(const std::string& action, const std::string& target, const Path& path, int priority)
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

  //  links[a + "--" + b] = LinkState();
  //  links[b + "--" + a] = LinkState();
}

std::vector<std::string> CentralizedTrafficController::choosePath(const Node& src, const Node& dst) const
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

Path CentralizedTrafficController::chooseServer(const Node& client, const std::vector<Path>& candidates)
{
    // TODO:
    // Replace with least-loaded, round-robin, latency-aware, or fairness-aware
    // server selection. This is the simplest actuation mechanism for a robust
    // bachelor-level load-balancing project.

    int currentRoundRobinCounter = roundRobinCounterEntries[client];

    int roundRobinIndex = currentRoundRobinCounter % static_cast<int>(candidates.size());
    const Path& chosenPath = candidates[roundRobinIndex];
    roundRobinCounterEntries[client] = currentRoundRobinCounter + 1;

    EV_INFO << "LOAD BALANCED: " << pathToString(chosenPath) << "\n";

    return chosenPath;
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

void CentralizedTrafficController::logAllPaths() const
{
    for (const auto& entry : allPaths) {
        EV_INFO << "Paths for " << entry.first << " (" << entry.second.size() << " total):\n";
        for (size_t i = 0; i < entry.second.size(); i++)
            EV_INFO << "  [" << i << "] " << pathToString(entry.second[i]) << "\n";
    }
}

void CentralizedTrafficController::shuffleAllPaths()
{
    std::mt19937 rng(std::random_device{}());
    for (auto& entry : allPaths)
        std::shuffle(entry.second.begin(), entry.second.end(), rng);
}

void CentralizedTrafficController::initRoundRobin()
{
    int i = 0;
    for (const auto& demand : demands)
        roundRobinCounterEntries[demand.first] = i++;
}

} // namespace sdnproject
