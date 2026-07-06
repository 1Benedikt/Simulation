message NetworkStateReport
{
    string reporter;
    string linkId;
    double utilization;
    int queueLength;
    double packetLossRate;
    bool failed;
    simtime_t measuredAt;
}

message ControlDecision
{
    string action;
    string target;
    string selectedPath;
    int priority;
    simtime_t issuedAt;
}
