#ifndef __SDNPROJECT_CENTRALIZEDTRAFFICCONTROLLER_H
#define __SDNPROJECT_CENTRALIZEDTRAFFICCONTROLLER_H

#include <omnetpp.h>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>


namespace sdnproject {

using Node = std::string;
using Path = std::vector<std::string>;


struct LinkState
{
    double utilization = 0.0;
    double queueDelay = 0.0;
    bool failed = false;
};

class CentralizedTrafficController : public omnetpp::cSimpleModule
{
  protected:
    omnetpp::simtime_t monitoringInterval;
    omnetpp::simtime_t minDecisionInterval;
    omnetpp::simtime_t lastDecisionTime;
    omnetpp::simsignal_t congestedLinkCountSignal;
    omnetpp::simsignal_t averageUtilizationSignal;
    omnetpp::simsignal_t failedLinksCountSignal;
    std::map<std::string, omnetpp::simsignal_t> routerLoadSignals;


    std::map<std::string, std::size_t> roundRobinCounters;          // per demand/client rotation index
    std::map<std::string, omnetpp::simtime_t> lastDemandDecisionTime; // per-demand cooldown
    std::map<std::string, Path> currentPaths;    // currently assigned path per demand
    std::map<std::string, std::set<std::string>> distinctPathsUsed;  // evaluation metric
    std::map<std::string, long> cumulativeFanout;
    long monitoringTicks = 0;


    std::string strategy;
    bool shufflePaths = false;
    double congestionThreshold = 0.75;
    long receivedReports = 0;
    long sentDecisions = 0;
    long pathSwitchCount = 0;
    std::vector<std::pair<std::string, std::string>> demands;
    std::map<std::string, LinkState> links;
    std::map<std::string, Path> topology;
    std::map<Node, Path> baselinePaths;
    std::map<std::string, std::vector<Path>> allPaths;
    std::map<Node, int> roundRobinCounterEntries;


  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;

    virtual void collectNetworkState();
    virtual void emitRouterLoad();
    virtual void processReport(omnetpp::cMessage *msg);
    virtual void applyBaselinePolicy();
    virtual void loadDemands();
    virtual std::vector<std::pair<std::string, std::string>> defaultDemands() const;
    virtual void runDynamicDecision();
    virtual void sendControlDecision(const std::string& action, const std::string& target, const Path& path, int priority);
    virtual void buildTopologyGraph();
    virtual void addBidirectionalLink(const std::string& a, const std::string& b);
    virtual std::vector<std::string> choosePath(const std::string& src, const std::string& dst) const;
    virtual Path chooseServer(const std::string& client, const std::vector<Path>& candidates);
    virtual double pathCost(const std::vector<std::string>& path) const;
    virtual std::string pathToString(const std::vector<std::string>& path) const;
    virtual void logAllPaths() const;
    virtual void initRoundRobin();
    virtual void shuffleAllPaths();
};

} // namespace sdnproject

#endif
