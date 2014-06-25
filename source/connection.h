#ifndef IPADDRESSINFO_H
#define IPADDRESSINFO_H

#include <QObject>
#include <QHostAddress>
#include <QUuid>

#include <QAbstractSocket>
#include "connectioninfo.h"

class QTcpServer;
class QTcpSocket;

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = 0);
    ~Connection();
    QHostAddress sourceAddress() const;
    QHostAddress serverAddress() const;
    quint16 sourcePort() const;
    quint16 serverPort() const;

    virtual void linkUp();
    virtual void linkDown();

    void setTcpConnected(bool);

    void send(const QByteArray &);

    ConnectionType connectionType() const;
    void setConnectionType(ConnectionType);

    QUuid id() const;

    bool isConnected() const;

    void setConnectionInfo(ConnectionInfo *conInfo);
    ConnectionInfo* connectionInfo() const;

signals:
    void sendedDataChanged(const QString &);
    void dataReceived(const QString &);
    void stateChanged(QAbstractSocket::SocketState socketState);
    void newConnectionEstablished(const QString&);
    void connectionClosed(const QString &);
private slots:
    void readPendingData();
    void processConnection();
    void processConnectionClose();
private:
    ConnectionInfo *info;
    QUuid uuid;
    bool connected;
    QObject *socket;
    QTcpSocket *connection;
};



#endif // IPADDRESSINFO_H
