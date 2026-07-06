package sdnproject.src.controller;

simple CentralizedTrafficController
{
    parameters:
        @class(sdnproject::CentralizedTrafficController);
        string strategy = default("baseline");
        double monitoringInterval @unit(s) = default(1s);
        double congestionThreshold = default(0.75);
        double minDecisionInterval @unit(s) = default(2s);
        string demandFile = default("simulations/demands.csv");
        @display("i=block/control");
    gates:
        input reportIn[];
        output decisionOut @loose;
}
