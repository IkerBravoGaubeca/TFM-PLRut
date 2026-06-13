//
// Generated file, do not edit! Created by opp_msgtool 6.0 from apps/PLRut/CapacityPackets.msg.
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
#include "CapacityPackets_m.h"

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

Register_Class(CapacityPacket)

CapacityPacket::CapacityPacket() : ::inet::FieldsChunk()
{
}

CapacityPacket::CapacityPacket(const CapacityPacket& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CapacityPacket::~CapacityPacket()
{
}

CapacityPacket& CapacityPacket::operator=(const CapacityPacket& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CapacityPacket::copy(const CapacityPacket& other)
{
    this->IDtalk = other.IDtalk;
    this->carId = other.carId;
    this->currentEdge = other.currentEdge;
    this->destinationEdge = other.destinationEdge;
    this->routeEdges = other.routeEdges;
    this->arrived = other.arrived;
    this->hasNewRoute = other.hasNewRoute;
    this->wait = other.wait;
    this->newRouteEdges = other.newRouteEdges;
}

void CapacityPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->IDtalk);
    doParsimPacking(b,this->carId);
    doParsimPacking(b,this->currentEdge);
    doParsimPacking(b,this->destinationEdge);
    doParsimPacking(b,this->routeEdges);
    doParsimPacking(b,this->arrived);
    doParsimPacking(b,this->hasNewRoute);
    doParsimPacking(b,this->wait);
    doParsimPacking(b,this->newRouteEdges);
}

void CapacityPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->IDtalk);
    doParsimUnpacking(b,this->carId);
    doParsimUnpacking(b,this->currentEdge);
    doParsimUnpacking(b,this->destinationEdge);
    doParsimUnpacking(b,this->routeEdges);
    doParsimUnpacking(b,this->arrived);
    doParsimUnpacking(b,this->hasNewRoute);
    doParsimUnpacking(b,this->wait);
    doParsimUnpacking(b,this->newRouteEdges);
}

unsigned int CapacityPacket::getIDtalk() const
{
    return this->IDtalk;
}

void CapacityPacket::setIDtalk(unsigned int IDtalk)
{
    handleChange();
    this->IDtalk = IDtalk;
}

const char * CapacityPacket::getCarId() const
{
    return this->carId.c_str();
}

void CapacityPacket::setCarId(const char * carId)
{
    handleChange();
    this->carId = carId;
}

const char * CapacityPacket::getCurrentEdge() const
{
    return this->currentEdge.c_str();
}

void CapacityPacket::setCurrentEdge(const char * currentEdge)
{
    handleChange();
    this->currentEdge = currentEdge;
}

const char * CapacityPacket::getDestinationEdge() const
{
    return this->destinationEdge.c_str();
}

void CapacityPacket::setDestinationEdge(const char * destinationEdge)
{
    handleChange();
    this->destinationEdge = destinationEdge;
}

const char * CapacityPacket::getRouteEdges() const
{
    return this->routeEdges.c_str();
}

void CapacityPacket::setRouteEdges(const char * routeEdges)
{
    handleChange();
    this->routeEdges = routeEdges;
}

bool CapacityPacket::getArrived() const
{
    return this->arrived;
}

void CapacityPacket::setArrived(bool arrived)
{
    handleChange();
    this->arrived = arrived;
}

bool CapacityPacket::getHasNewRoute() const
{
    return this->hasNewRoute;
}

void CapacityPacket::setHasNewRoute(bool hasNewRoute)
{
    handleChange();
    this->hasNewRoute = hasNewRoute;
}

bool CapacityPacket::getWait() const
{
    return this->wait;
}

void CapacityPacket::setWait(bool wait)
{
    handleChange();
    this->wait = wait;
}

const char * CapacityPacket::getNewRouteEdges() const
{
    return this->newRouteEdges.c_str();
}

void CapacityPacket::setNewRouteEdges(const char * newRouteEdges)
{
    handleChange();
    this->newRouteEdges = newRouteEdges;
}

class CapacityPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_IDtalk,
        FIELD_carId,
        FIELD_currentEdge,
        FIELD_destinationEdge,
        FIELD_routeEdges,
        FIELD_arrived,
        FIELD_hasNewRoute,
        FIELD_wait,
        FIELD_newRouteEdges,
    };
  public:
    CapacityPacketDescriptor();
    virtual ~CapacityPacketDescriptor();

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

Register_ClassDescriptor(CapacityPacketDescriptor)

CapacityPacketDescriptor::CapacityPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(CapacityPacket)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CapacityPacketDescriptor::~CapacityPacketDescriptor()
{
    delete[] propertyNames;
}

bool CapacityPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CapacityPacket *>(obj)!=nullptr;
}

const char **CapacityPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CapacityPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CapacityPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 9+base->getFieldCount() : 9;
}

unsigned int CapacityPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_IDtalk
        FD_ISEDITABLE,    // FIELD_carId
        FD_ISEDITABLE,    // FIELD_currentEdge
        FD_ISEDITABLE,    // FIELD_destinationEdge
        FD_ISEDITABLE,    // FIELD_routeEdges
        FD_ISEDITABLE,    // FIELD_arrived
        FD_ISEDITABLE,    // FIELD_hasNewRoute
        FD_ISEDITABLE,    // FIELD_wait
        FD_ISEDITABLE,    // FIELD_newRouteEdges
    };
    return (field >= 0 && field < 9) ? fieldTypeFlags[field] : 0;
}

const char *CapacityPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "IDtalk",
        "carId",
        "currentEdge",
        "destinationEdge",
        "routeEdges",
        "arrived",
        "hasNewRoute",
        "wait",
        "newRouteEdges",
    };
    return (field >= 0 && field < 9) ? fieldNames[field] : nullptr;
}

int CapacityPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "IDtalk") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "carId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "currentEdge") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "destinationEdge") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "routeEdges") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "arrived") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "hasNewRoute") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "wait") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "newRouteEdges") == 0) return baseIndex + 8;
    return base ? base->findField(fieldName) : -1;
}

const char *CapacityPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned int",    // FIELD_IDtalk
        "string",    // FIELD_carId
        "string",    // FIELD_currentEdge
        "string",    // FIELD_destinationEdge
        "string",    // FIELD_routeEdges
        "bool",    // FIELD_arrived
        "bool",    // FIELD_hasNewRoute
        "bool",    // FIELD_wait
        "string",    // FIELD_newRouteEdges
    };
    return (field >= 0 && field < 9) ? fieldTypeStrings[field] : nullptr;
}

const char **CapacityPacketDescriptor::getFieldPropertyNames(int field) const
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

const char *CapacityPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int CapacityPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CapacityPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CapacityPacket'", field);
    }
}

const char *CapacityPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CapacityPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        case FIELD_IDtalk: return ulong2string(pp->getIDtalk());
        case FIELD_carId: return oppstring2string(pp->getCarId());
        case FIELD_currentEdge: return oppstring2string(pp->getCurrentEdge());
        case FIELD_destinationEdge: return oppstring2string(pp->getDestinationEdge());
        case FIELD_routeEdges: return oppstring2string(pp->getRouteEdges());
        case FIELD_arrived: return bool2string(pp->getArrived());
        case FIELD_hasNewRoute: return bool2string(pp->getHasNewRoute());
        case FIELD_wait: return bool2string(pp->getWait());
        case FIELD_newRouteEdges: return oppstring2string(pp->getNewRouteEdges());
        default: return "";
    }
}

void CapacityPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        case FIELD_IDtalk: pp->setIDtalk(string2ulong(value)); break;
        case FIELD_carId: pp->setCarId((value)); break;
        case FIELD_currentEdge: pp->setCurrentEdge((value)); break;
        case FIELD_destinationEdge: pp->setDestinationEdge((value)); break;
        case FIELD_routeEdges: pp->setRouteEdges((value)); break;
        case FIELD_arrived: pp->setArrived(string2bool(value)); break;
        case FIELD_hasNewRoute: pp->setHasNewRoute(string2bool(value)); break;
        case FIELD_wait: pp->setWait(string2bool(value)); break;
        case FIELD_newRouteEdges: pp->setNewRouteEdges((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CapacityPacket'", field);
    }
}

omnetpp::cValue CapacityPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        case FIELD_IDtalk: return (omnetpp::intval_t)(pp->getIDtalk());
        case FIELD_carId: return pp->getCarId();
        case FIELD_currentEdge: return pp->getCurrentEdge();
        case FIELD_destinationEdge: return pp->getDestinationEdge();
        case FIELD_routeEdges: return pp->getRouteEdges();
        case FIELD_arrived: return pp->getArrived();
        case FIELD_hasNewRoute: return pp->getHasNewRoute();
        case FIELD_wait: return pp->getWait();
        case FIELD_newRouteEdges: return pp->getNewRouteEdges();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CapacityPacket' as cValue -- field index out of range?", field);
    }
}

void CapacityPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        case FIELD_IDtalk: pp->setIDtalk(omnetpp::checked_int_cast<unsigned int>(value.intValue())); break;
        case FIELD_carId: pp->setCarId(value.stringValue()); break;
        case FIELD_currentEdge: pp->setCurrentEdge(value.stringValue()); break;
        case FIELD_destinationEdge: pp->setDestinationEdge(value.stringValue()); break;
        case FIELD_routeEdges: pp->setRouteEdges(value.stringValue()); break;
        case FIELD_arrived: pp->setArrived(value.boolValue()); break;
        case FIELD_hasNewRoute: pp->setHasNewRoute(value.boolValue()); break;
        case FIELD_wait: pp->setWait(value.boolValue()); break;
        case FIELD_newRouteEdges: pp->setNewRouteEdges(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CapacityPacket'", field);
    }
}

const char *CapacityPacketDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CapacityPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CapacityPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CapacityPacket *pp = omnetpp::fromAnyPtr<CapacityPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CapacityPacket'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

