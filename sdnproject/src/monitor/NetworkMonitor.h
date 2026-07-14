#ifndef __SDNPROJECT_NETWORKMONITOR_H
#define __SDNPROJECT_NETWORKMONITOR_H

#include <omnetpp.h>
#include <string>

namespace sdnproject {

class NetworkMonitor : public omnetpp::cSimpleModule
{
  protected:
    omnetpp::simtime_t reportInterval;
    std::string monitoredNode;


  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;

    virtual void sendReport();
};

} // namespace sdnproject

#endif
