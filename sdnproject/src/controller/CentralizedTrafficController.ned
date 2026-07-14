package sdnproject.src.controller;

simple CentralizedTrafficController
{
    parameters:
        @signal[congestedLinkCount](type=long);
  		@statistic[congestedLinkCount](record=vector,mean,max);
  		@signal[averageUtilization](type=double);
  		@statistic[averageUtilization](record=vector,mean);
  		@signal[failedLinksCount](type=long);
  		@statistic[failedLinksCount](record=vector,mean,max,min);
  		

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
