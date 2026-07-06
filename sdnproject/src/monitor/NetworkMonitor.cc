#include "NetworkMonitor.h"

#include "../messages/ControlMessages_m.h"

#include <algorithm>

using namespace omnetpp;
namespace sdnproject {

Define_Module(NetworkMonitor);

void NetworkMonitor::initialize()
{
    reportInterval = par("reportInterval").doubleValue();
    monitoredNode = par("monitoredNode").stdstringValue();

    auto *timer = new cMessage("sendReport");
    scheduleAt(simTime() + uniform(0, reportInterval.dbl()), timer);
}

void NetworkMonitor::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        sendReport();
        scheduleAt(simTime() + reportInterval, msg);
        return;
    }

    delete msg;
}

void NetworkMonitor::finish()
{
}

void NetworkMonitor::sendReport()
{
    auto *report = new NetworkStateReport("networkStateReport");
    double base = par("baseUtilization").doubleValue();
    double jitter = par("utilizationJitter").doubleValue();
    double utilization = std::max(0.0, std::min(1.0, base + uniform(-jitter, jitter)));

    report->setReporter(monitoredNode.c_str());
    report->setLinkId((monitoredNode + "-uplink").c_str());
    report->setUtilization(utilization);
    report->setQueueLength(par("baseQueueLength").intValue());
    report->setPacketLossRate(utilization > 0.85 ? 0.02 : 0.0);
    report->setFailed(par("failed").boolValue());
    report->setMeasuredAt(simTime());

    send(report, "out");
}

} // namespace sdnproject
