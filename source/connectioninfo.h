#ifndef CONNECTIONINFOTYPE_H
#define CONNECTIONINFOTYPE_H
#include <QHostAddress>

enum ConnectionType {CONNECTION_UDP_CLIENT, CONNECTION_UDP_SERVER, CONNECTION_TCP_CLIENT, CONNECTION_TCP_SERVER};

struct ConnectionInfo
{
    QHostAddress dstAddress;
    QHostAddress srcAddress;
    quint16 srcPort;
    quint16 dstPort;
    ConnectionType conType;
    bool randomSrcPort;
};

QDataStream& operator << (QDataStream &stream, const ConnectionInfo &info);
QDataStream& operator >> (QDataStream &stream, ConnectionInfo &info);

bool operator == (ConnectionInfo &a, ConnectionInfo &b);

Q_DECLARE_METATYPE(ConnectionInfo)

#endif // CONNECTIONINFOTYPE_H
