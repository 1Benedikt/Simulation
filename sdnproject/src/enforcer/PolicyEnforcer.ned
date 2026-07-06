package sdnproject.src.enforcer;

simple PolicyEnforcer
{
    parameters:
        @class(sdnproject::PolicyEnforcer);
        @display("i=block/process");
    gates:
        input in @loose;
}
