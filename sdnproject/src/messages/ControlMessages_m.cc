//
// Generated file, do not edit! Created by opp_msgtool 6.3 from messages/ControlMessages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "ControlMessages_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(NetworkStateReport)

NetworkStateReport::NetworkStateReport(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

NetworkStateReport::NetworkStateReport(const NetworkStateReport& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

NetworkStateReport::~NetworkStateReport()
{
}

NetworkStateReport& NetworkStateReport::operator=(const NetworkStateReport& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void NetworkStateReport::copy(const NetworkStateReport& other)
{
    this->reporter = other.reporter;
    this->linkId = other.linkId;
    this->utilization = other.utilization;
    this->queueLength = other.queueLength;
    this->packetLossRate = other.packetLossRate;
    this->failed = other.failed;
    this->measuredAt = other.measuredAt;
}

void NetworkStateReport::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->reporter);
    doParsimPacking(b,this->linkId);
    doParsimPacking(b,this->utilization);
    doParsimPacking(b,this->queueLength);
    doParsimPacking(b,this->packetLossRate);
    doParsimPacking(b,this->failed);
    doParsimPacking(b,this->measuredAt);
}

void NetworkStateReport::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->reporter);
    doParsimUnpacking(b,this->linkId);
    doParsimUnpacking(b,this->utilization);
    doParsimUnpacking(b,this->queueLength);
    doParsimUnpacking(b,this->packetLossRate);
    doParsimUnpacking(b,this->failed);
    doParsimUnpacking(b,this->measuredAt);
}

const char * NetworkStateReport::getReporter() const
{
    return this->reporter.c_str();
}

void NetworkStateReport::setReporter(const char * reporter)
{
    this->reporter = reporter;
}

const char * NetworkStateReport::getLinkId() const
{
    return this->linkId.c_str();
}

void NetworkStateReport::setLinkId(const char * linkId)
{
    this->linkId = linkId;
}

double NetworkStateReport::getUtilization() const
{
    return this->utilization;
}

void NetworkStateReport::setUtilization(double utilization)
{
    this->utilization = utilization;
}

int NetworkStateReport::getQueueLength() const
{
    return this->queueLength;
}

void NetworkStateReport::setQueueLength(int queueLength)
{
    this->queueLength = queueLength;
}

double NetworkStateReport::getPacketLossRate() const
{
    return this->packetLossRate;
}

void NetworkStateReport::setPacketLossRate(double packetLossRate)
{
    this->packetLossRate = packetLossRate;
}

bool NetworkStateReport::getFailed() const
{
    return this->failed;
}

void NetworkStateReport::setFailed(bool failed)
{
    this->failed = failed;
}

omnetpp::simtime_t NetworkStateReport::getMeasuredAt() const
{
    return this->measuredAt;
}

void NetworkStateReport::setMeasuredAt(omnetpp::simtime_t measuredAt)
{
    this->measuredAt = measuredAt;
}

class NetworkStateReportDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_reporter,
        FIELD_linkId,
        FIELD_utilization,
        FIELD_queueLength,
        FIELD_packetLossRate,
        FIELD_failed,
        FIELD_measuredAt,
    };
  public:
    NetworkStateReportDescriptor();
    virtual ~NetworkStateReportDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(NetworkStateReportDescriptor)

NetworkStateReportDescriptor::NetworkStateReportDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(NetworkStateReport)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

NetworkStateReportDescriptor::~NetworkStateReportDescriptor()
{
    delete[] propertyNames;
}

bool NetworkStateReportDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<NetworkStateReport *>(obj)!=nullptr;
}

const char **NetworkStateReportDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *NetworkStateReportDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int NetworkStateReportDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 7+base->getFieldCount() : 7;
}

unsigned int NetworkStateReportDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_reporter
        FD_ISEDITABLE,    // FIELD_linkId
        FD_ISEDITABLE,    // FIELD_utilization
        FD_ISEDITABLE,    // FIELD_queueLength
        FD_ISEDITABLE,    // FIELD_packetLossRate
        FD_ISEDITABLE,    // FIELD_failed
        FD_ISEDITABLE,    // FIELD_measuredAt
    };
    return (field >= 0 && field < 7) ? fieldTypeFlags[field] : 0;
}

const char *NetworkStateReportDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "reporter",
        "linkId",
        "utilization",
        "queueLength",
        "packetLossRate",
        "failed",
        "measuredAt",
    };
    return (field >= 0 && field < 7) ? fieldNames[field] : nullptr;
}

int NetworkStateReportDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "reporter") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "linkId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "utilization") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "queueLength") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "packetLossRate") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "failed") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "measuredAt") == 0) return baseIndex + 6;
    return base ? base->findField(fieldName) : -1;
}

const char *NetworkStateReportDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_reporter
        "string",    // FIELD_linkId
        "double",    // FIELD_utilization
        "int",    // FIELD_queueLength
        "double",    // FIELD_packetLossRate
        "bool",    // FIELD_failed
        "omnetpp::simtime_t",    // FIELD_measuredAt
    };
    return (field >= 0 && field < 7) ? fieldTypeStrings[field] : nullptr;
}

const char **NetworkStateReportDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *NetworkStateReportDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int NetworkStateReportDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void NetworkStateReportDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'NetworkStateReport'", field);
    }
}

const char *NetworkStateReportDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string NetworkStateReportDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        case FIELD_reporter: return oppstring2string(pp->getReporter());
        case FIELD_linkId: return oppstring2string(pp->getLinkId());
        case FIELD_utilization: return double2string(pp->getUtilization());
        case FIELD_queueLength: return long2string(pp->getQueueLength());
        case FIELD_packetLossRate: return double2string(pp->getPacketLossRate());
        case FIELD_failed: return bool2string(pp->getFailed());
        case FIELD_measuredAt: return simtime2string(pp->getMeasuredAt());
        default: return "";
    }
}

void NetworkStateReportDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        case FIELD_reporter: pp->setReporter((value)); break;
        case FIELD_linkId: pp->setLinkId((value)); break;
        case FIELD_utilization: pp->setUtilization(string2double(value)); break;
        case FIELD_queueLength: pp->setQueueLength(string2long(value)); break;
        case FIELD_packetLossRate: pp->setPacketLossRate(string2double(value)); break;
        case FIELD_failed: pp->setFailed(string2bool(value)); break;
        case FIELD_measuredAt: pp->setMeasuredAt(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'NetworkStateReport'", field);
    }
}

omnetpp::cValue NetworkStateReportDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        case FIELD_reporter: return pp->getReporter();
        case FIELD_linkId: return pp->getLinkId();
        case FIELD_utilization: return pp->getUtilization();
        case FIELD_queueLength: return pp->getQueueLength();
        case FIELD_packetLossRate: return pp->getPacketLossRate();
        case FIELD_failed: return pp->getFailed();
        case FIELD_measuredAt: return pp->getMeasuredAt().dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'NetworkStateReport' as cValue -- field index out of range?", field);
    }
}

void NetworkStateReportDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        case FIELD_reporter: pp->setReporter(value.stringValue()); break;
        case FIELD_linkId: pp->setLinkId(value.stringValue()); break;
        case FIELD_utilization: pp->setUtilization(value.doubleValue()); break;
        case FIELD_queueLength: pp->setQueueLength(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_packetLossRate: pp->setPacketLossRate(value.doubleValue()); break;
        case FIELD_failed: pp->setFailed(value.boolValue()); break;
        case FIELD_measuredAt: pp->setMeasuredAt(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'NetworkStateReport'", field);
    }
}

const char *NetworkStateReportDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr NetworkStateReportDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void NetworkStateReportDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    NetworkStateReport *pp = omnetpp::fromAnyPtr<NetworkStateReport>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'NetworkStateReport'", field);
    }
}

Register_Class(ControlDecision)

ControlDecision::ControlDecision(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

ControlDecision::ControlDecision(const ControlDecision& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ControlDecision::~ControlDecision()
{
}

ControlDecision& ControlDecision::operator=(const ControlDecision& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ControlDecision::copy(const ControlDecision& other)
{
    this->action = other.action;
    this->target = other.target;
    this->selectedPath = other.selectedPath;
    this->priority = other.priority;
    this->issuedAt = other.issuedAt;
}

void ControlDecision::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->action);
    doParsimPacking(b,this->target);
    doParsimPacking(b,this->selectedPath);
    doParsimPacking(b,this->priority);
    doParsimPacking(b,this->issuedAt);
}

void ControlDecision::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->action);
    doParsimUnpacking(b,this->target);
    doParsimUnpacking(b,this->selectedPath);
    doParsimUnpacking(b,this->priority);
    doParsimUnpacking(b,this->issuedAt);
}

const char * ControlDecision::getAction() const
{
    return this->action.c_str();
}

void ControlDecision::setAction(const char * action)
{
    this->action = action;
}

const char * ControlDecision::getTarget() const
{
    return this->target.c_str();
}

void ControlDecision::setTarget(const char * target)
{
    this->target = target;
}

const char * ControlDecision::getSelectedPath() const
{
    return this->selectedPath.c_str();
}

void ControlDecision::setSelectedPath(const char * selectedPath)
{
    this->selectedPath = selectedPath;
}

int ControlDecision::getPriority() const
{
    return this->priority;
}

void ControlDecision::setPriority(int priority)
{
    this->priority = priority;
}

omnetpp::simtime_t ControlDecision::getIssuedAt() const
{
    return this->issuedAt;
}

void ControlDecision::setIssuedAt(omnetpp::simtime_t issuedAt)
{
    this->issuedAt = issuedAt;
}

class ControlDecisionDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_action,
        FIELD_target,
        FIELD_selectedPath,
        FIELD_priority,
        FIELD_issuedAt,
    };
  public:
    ControlDecisionDescriptor();
    virtual ~ControlDecisionDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ControlDecisionDescriptor)

ControlDecisionDescriptor::ControlDecisionDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ControlDecision)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

ControlDecisionDescriptor::~ControlDecisionDescriptor()
{
    delete[] propertyNames;
}

bool ControlDecisionDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ControlDecision *>(obj)!=nullptr;
}

const char **ControlDecisionDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ControlDecisionDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ControlDecisionDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int ControlDecisionDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_action
        FD_ISEDITABLE,    // FIELD_target
        FD_ISEDITABLE,    // FIELD_selectedPath
        FD_ISEDITABLE,    // FIELD_priority
        FD_ISEDITABLE,    // FIELD_issuedAt
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *ControlDecisionDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "action",
        "target",
        "selectedPath",
        "priority",
        "issuedAt",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int ControlDecisionDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "action") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "target") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "selectedPath") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "priority") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "issuedAt") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *ControlDecisionDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_action
        "string",    // FIELD_target
        "string",    // FIELD_selectedPath
        "int",    // FIELD_priority
        "omnetpp::simtime_t",    // FIELD_issuedAt
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **ControlDecisionDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ControlDecisionDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ControlDecisionDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ControlDecisionDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ControlDecision'", field);
    }
}

const char *ControlDecisionDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ControlDecisionDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        case FIELD_action: return oppstring2string(pp->getAction());
        case FIELD_target: return oppstring2string(pp->getTarget());
        case FIELD_selectedPath: return oppstring2string(pp->getSelectedPath());
        case FIELD_priority: return long2string(pp->getPriority());
        case FIELD_issuedAt: return simtime2string(pp->getIssuedAt());
        default: return "";
    }
}

void ControlDecisionDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        case FIELD_action: pp->setAction((value)); break;
        case FIELD_target: pp->setTarget((value)); break;
        case FIELD_selectedPath: pp->setSelectedPath((value)); break;
        case FIELD_priority: pp->setPriority(string2long(value)); break;
        case FIELD_issuedAt: pp->setIssuedAt(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControlDecision'", field);
    }
}

omnetpp::cValue ControlDecisionDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        case FIELD_action: return pp->getAction();
        case FIELD_target: return pp->getTarget();
        case FIELD_selectedPath: return pp->getSelectedPath();
        case FIELD_priority: return pp->getPriority();
        case FIELD_issuedAt: return pp->getIssuedAt().dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ControlDecision' as cValue -- field index out of range?", field);
    }
}

void ControlDecisionDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        case FIELD_action: pp->setAction(value.stringValue()); break;
        case FIELD_target: pp->setTarget(value.stringValue()); break;
        case FIELD_selectedPath: pp->setSelectedPath(value.stringValue()); break;
        case FIELD_priority: pp->setPriority(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_issuedAt: pp->setIssuedAt(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControlDecision'", field);
    }
}

const char *ControlDecisionDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ControlDecisionDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ControlDecisionDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ControlDecision *pp = omnetpp::fromAnyPtr<ControlDecision>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ControlDecision'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

