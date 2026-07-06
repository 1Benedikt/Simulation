package sdnproject.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import inet.node.inet.Router;
import sdnproject.src.controller.CentralizedTrafficController;
import sdnproject.src.enforcer.PolicyEnforcer;
import sdnproject.src.monitor.NetworkMonitor;

//
// A small datacenter-style topology for SDN-inspired centralized control:
// - 2 core forwarding nodes
// - 4 aggregation forwarding nodes
// - 4 edge forwarding nodes
// - 16 hosts
// - 1 controller module representing the centralized decision logic
// - 10 monitor modules that send abstract state reports to the controller
// - 1 policy enforcer that receives abstract controller decisions
//
// This model does not simulate OpenFlow messages or switch flow tables. Instead,
// it gives students a stable INET topology in which they can implement and
// evaluate centralized traffic-engineering decisions.
//
network DatacenterCentralizedControl
{
    parameters:
        @display("bgb=1200,760");

    types:
        channel DataLink extends ned.DatarateChannel
        {
            datarate = 100Mbps;
            delay = 10us;
        }

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=120,70;i=block/cogwheel");
        }

        controller: CentralizedTrafficController {
            @display("p=600,70;i=device/server");
        }

        enforcer: PolicyEnforcer {
            @display("p=760,70;i=block/process");
        }

        monitor[10]: NetworkMonitor {
            @display("p=350+45*i,90;i=block/source");
        }

        core[2]: Router {
            @display("p=450+300*i,180;i=device/switch");
        }

        aggregation[4]: Router {
            @display("p=240+240*i,340;i=device/switch");
        }

        edge[4]: Router {
            @display("p=240+240*i,510;i=device/switch");
        }

        host[16]: StandardHost {
            @display("p=105+70*(i%4)+240*(i/4),650;i=device/pc");
        }

    connections allowunconnected:
        for i=0..9 {
            monitor[i].out --> controller.reportIn++;
        }
        controller.decisionOut --> enforcer.in;

        // Core to aggregation: full mesh for path diversity.
        core[0].ethg++ <--> DataLink <--> aggregation[0].ethg++;
        core[0].ethg++ <--> DataLink <--> aggregation[1].ethg++;
        core[0].ethg++ <--> DataLink <--> aggregation[2].ethg++;
        core[0].ethg++ <--> DataLink <--> aggregation[3].ethg++;
        core[1].ethg++ <--> DataLink <--> aggregation[0].ethg++;
        core[1].ethg++ <--> DataLink <--> aggregation[1].ethg++;
        core[1].ethg++ <--> DataLink <--> aggregation[2].ethg++;
        core[1].ethg++ <--> DataLink <--> aggregation[3].ethg++;

        // Aggregation to edge: two pods, each with two aggregation and two edge nodes.
        aggregation[0].ethg++ <--> DataLink <--> edge[0].ethg++;
        aggregation[0].ethg++ <--> DataLink <--> edge[1].ethg++;
        aggregation[1].ethg++ <--> DataLink <--> edge[0].ethg++;
        aggregation[1].ethg++ <--> DataLink <--> edge[1].ethg++;

        aggregation[2].ethg++ <--> DataLink <--> edge[2].ethg++;
        aggregation[2].ethg++ <--> DataLink <--> edge[3].ethg++;
        aggregation[3].ethg++ <--> DataLink <--> edge[2].ethg++;
        aggregation[3].ethg++ <--> DataLink <--> edge[3].ethg++;

        // Hosts.
        for i=0..3 {
            host[i].ethg++ <--> DataLink <--> edge[0].ethg++;
            host[i + 4].ethg++ <--> DataLink <--> edge[1].ethg++;
            host[i + 8].ethg++ <--> DataLink <--> edge[2].ethg++;
            host[i + 12].ethg++ <--> DataLink <--> edge[3].ethg++;
        }
}
