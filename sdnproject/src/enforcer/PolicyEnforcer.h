#ifndef __SDNPROJECT_POLICYENFORCER_H
#define __SDNPROJECT_POLICYENFORCER_H

#include <omnetpp.h>
#include <map>
#include <string>

namespace sdnproject {

class PolicyEnforcer : public omnetpp::cSimpleModule
{
  protected:
    std::map<std::string, std::string> activePolicies;
    long receivedDecisions = 0;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
};

} // namespace sdnproject

#endif
