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

        @signal[routerLoadCore0](type=long);
        @statistic[routerLoadCore0](record=vector,mean,max; title="Active demands through core[0]");
        @signal[routerLoadCore1](type=long);
        @statistic[routerLoadCore1](record=vector,mean,max; title="Active demands through core[1]");
        @signal[routerLoadAggregation0](type=long);
        @statistic[routerLoadAggregation0](record=vector,mean,max; title="Active demands through aggregation[0]");
        @signal[routerLoadAggregation1](type=long);
        @statistic[routerLoadAggregation1](record=vector,mean,max; title="Active demands through aggregation[1]");
        @signal[routerLoadAggregation2](type=long);
        @statistic[routerLoadAggregation2](record=vector,mean,max; title="Active demands through aggregation[2]");
        @signal[routerLoadAggregation3](type=long);
        @statistic[routerLoadAggregation3](record=vector,mean,max; title="Active demands through aggregation[3]");
        @signal[routerLoadEdge0](type=long);
        @statistic[routerLoadEdge0](record=vector,mean,max; title="Active demands through edge[0]");
        @signal[routerLoadEdge1](type=long);
        @statistic[routerLoadEdge1](record=vector,mean,max; title="Active demands through edge[1]");
        @signal[routerLoadEdge2](type=long);
        @statistic[routerLoadEdge2](record=vector,mean,max; title="Active demands through edge[2]");
        @signal[routerLoadEdge3](type=long);
        @statistic[routerLoadEdge3](record=vector,mean,max; title="Active demands through edge[3]");

        @class(sdnproject::CentralizedTrafficController);
        string strategy = default("baseline");
        double monitoringInterval @unit(s) = default(1s);
        double congestionThreshold = default(0.75);
        double minDecisionInterval @unit(s) = default(2s);
        string demandFile = default("simulations/demands.csv");
        bool shufflePaths = default(false);
        @display("i=block/control");
    gates:
        input reportIn[];
        output decisionOut @loose;
}
