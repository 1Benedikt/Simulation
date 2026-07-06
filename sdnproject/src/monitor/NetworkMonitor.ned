package sdnproject.src.monitor;

simple NetworkMonitor
{
    parameters:
        @class(sdnproject::NetworkMonitor);
        string monitoredNode = default("");
        double reportInterval @unit(s) = default(1s);
        double baseUtilization = default(0.2);
        double utilizationJitter = default(0.1);
        int baseQueueLength = default(0);
        bool failed = default(false);
        @display("i=block/source");
    gates:
        output out;
}
