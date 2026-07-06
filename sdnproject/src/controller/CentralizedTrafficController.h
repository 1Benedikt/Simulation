#ifndef __SDNPROJECT_CENTRALIZEDTRAFFICCONTROLLER_H
#define __SDNPROJECT_CENTRALIZEDTRAFFICCONTROLLER_H

#include <omnetpp.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace sdnproject {

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
    std::string strategy;
    double congestionThreshold = 0.75;
    long receivedReports = 0;
    long sentDecisions = 0;
    std::vector<std::pair<std::string, std::string>> demands;
    std::map<std::string, LinkState> links;
    std::map<std::string, std::vector<std::string>> topology;
    std::map<std::string, std::vector<std::string>> baselinePaths;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;

    virtual void collectNetworkState();
    virtual void processReport(omnetpp::cMessage *msg);
    virtual void applyBaselinePolicy();
    virtual void loadDemands();
    virtual std::vector<std::pair<std::string, std::string>> defaultDemands() const;
    virtual void runDynamicDecision();
    virtual void sendControlDecision(const std::string& action, const std::string& target, const std::vector<std::string>& path, int priority);
    virtual void buildTopologyGraph();
    virtual void addBidirectionalLink(const std::string& a, const std::string& b);
    virtual std::vector<std::string> choosePath(const std::string& src, const std::string& dst) const;
    virtual std::string chooseServer(const std::string& client, const std::vector<std::string>& candidates) const;
    virtual double pathCost(const std::vector<std::string>& path) const;
    virtual std::string pathToString(const std::vector<std::string>& path) const;
};

} // namespace sdnproject

#endif
