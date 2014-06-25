#include "connectioninfo.h"

QDataStream& operator << (QDataStream &stream, const ConnectionInfo &info)
{
    stream << info.dstAddress << info.dstPort
           << info.srcAddress << info.srcPort
           << (quint8)info.conType << info.randomSrcPort;
    return stream;
}

QDataStream& operator >> (QDataStream &stream, ConnectionInfo &info)
{
    quint8 type;
    stream >> info.dstAddress >> info.dstPort
           >> info.srcAddress >> info.srcPort
           >> type
           >> info.randomSrcPort;
    info.conType = static_cast<ConnectionType>(type);
    return stream;
}

bool operator == (ConnectionInfo &a, ConnectionInfo &b)
{
    bool equal = false;
    switch(a.conType) {
    case CONNECTION_UDP_CLIENT:
    case CONNECTION_TCP_CLIENT:
        if (b.conType == a.conType && a.randomSrcPort == b.randomSrcPort && a.dstAddress == b.dstAddress
            && a.srcAddress == b.srcAddress && a.dstPort == b.dstPort) {
            if (a.randomSrcPort == true || (a.randomSrcPort != true && a.srcPort == b.srcPort))
                equal = true;
        }
    break;
    case CONNECTION_UDP_SERVER:
    case CONNECTION_TCP_SERVER:
        if (b.conType == a.conType && a.dstAddress == b.dstAddress && a.dstPort == b.dstPort)
            equal = true;
    break;
    }
    return equal;
}
