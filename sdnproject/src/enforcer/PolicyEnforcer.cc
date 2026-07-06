#include "PolicyEnforcer.h"

#include "../messages/ControlMessages_m.h"

using namespace omnetpp;
namespace sdnproject {

Define_Module(PolicyEnforcer);

void PolicyEnforcer::initialize()
{
}

void PolicyEnforcer::handleMessage(cMessage *msg)
{
    auto *decision = dynamic_cast<ControlDecision *>(msg);
    if (decision == nullptr) {
        delete msg;
        return;
    }

    receivedDecisions++;
    activePolicies[decision->getTarget()] = decision->getSelectedPath();

    EV_INFO << "PolicyEnforcer applied action=" << decision->getAction()
            << " target=" << decision->getTarget()
            << " selectedPath=" << decision->getSelectedPath()
            << " priority=" << decision->getPriority() << "\n";

    delete decision;
}

void PolicyEnforcer::finish()
{
    recordScalar("receivedControlDecisions", receivedDecisions);
    recordScalar("activePolicies", static_cast<long>(activePolicies.size()));
}

} // namespace sdnproject
