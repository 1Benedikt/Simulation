# Controller Extension Point

`CentralizedTrafficController` contains the algorithmic skeleton for the
project.

This is an SDN-inspired abstraction, not a protocol-level OpenFlow model. The
controller represents logically centralized network control. Students should
choose a clear actuation mechanism and document it.

The important student contribution is the decision-making mechanism:

- What state is monitored?
- When does the controller react?
- Which path, server, route, or priority class is selected?
- Which policy-enforcement module applies the decision?
- How are oscillations avoided?
- How is the proposed strategy compared against the baseline?

Recommended simple algorithms:

- Static shortest path baseline
- Round-robin path selection among equal-cost paths
- Utilization-aware routing using port statistics
- Failure-aware rerouting after a link is disabled
- QoS-aware routing that keeps latency-sensitive traffic away from bulk flows
- Server selection among replicated destination hosts

The provided skeleton already supports two abstract message types:

- `NetworkStateReport`: monitor-to-controller state signaling
- `ControlDecision`: controller-to-enforcer policy signaling

The baseline demand pairs are loaded from `data/demands.csv` through the
`demandFile` parameter. This gives students a simple traffic-matrix input that
they can change per scenario without editing C++ code.
