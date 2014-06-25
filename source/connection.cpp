#include "connection.h"
#include <QtNetwork>
#include <QMessageBox>

Connection::Connection(QObject *parent) :
    QObject(parent)
{
    info = NULL;
    uuid = QUuid::createUuid();
    qDebug("connection %s created", qPrintable(uuid.toString()));
}


QHostAddress Connection::sourceAddress() const
{
    return info->srcAddress;
}

QHostAddress Connection::serverAddress() const
{
    return info->dstAddress;
}

void Connection::setTcpConnected(bool state)
{
    switch(info->conType) {
    case CONNECTION_TCP_CLIENT:
    {
        QTcpSocket *tcpSocket = qobject_cast<QTcpSocket *>(socket);
        if (state) {
            if (tcpSocket->state() != QAbstractSocket::ConnectingState) {
                tcpSocket->connectToHost(info->dstAddress, info->dstPort);
                qDebug("tcp connecting...");
            }
        } else {
            tcpSocket->disconnectFromHost();
            qDebug("closing tcp connection...");
        }
    }
        break;
    default:
        break;
    }
}

void Connection::linkUp()
{
    switch(info->conType) {
    case CONNECTION_UDP_CLIENT:
    {
        socket = new QUdpSocket(this);
        qDebug("making udp client");

        QUdpSocket *udpSocket = qobject_cast<QUdpSocket *>(socket);
        if (info->randomSrcPort) {
            info->srcPort = qrand() / (float)RAND_MAX * 65535;
            qDebug("random source port: %d", info->srcPort);
        }

        bool success = udpSocket->bind(info->srcPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        {
            if (!success) {
                qCritical("udp port %d bind error", info->srcPort);
                QMessageBox::critical(NULL, tr("Network Error"),
                                      tr("Error to bind port %1. May it be occupied by another program?").arg(info->dstPort));
                return;
            } else {
                qDebug("udp bind to port %d", info->srcPort);
            }
        }
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingData()));
        qDebug("udp client made");
    }
        break;
    case CONNECTION_TCP_CLIENT:
        socket = new QTcpSocket(this);
        qDebug("making tcp client");
        if (info->randomSrcPort) {
            info->srcPort = qrand() / (float)RAND_MAX * 65535;
            qDebug("random source port: %d", info->srcPort);
        }

        setTcpConnected(true);
        connect(socket, SIGNAL(readyRead()), this, SLOT(readPendingData()));
        connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SIGNAL(stateChanged(QAbstractSocket::SocketState)));
        qDebug("tcp client made");
        break;
    case CONNECTION_UDP_SERVER:
    {
        socket = new QUdpSocket(this);
        qDebug("making udp server");
        bool success = ((QUdpSocket*)socket)->bind(info->dstPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        if (!success) {
            qCritical("failed to bind to port %d", info->dstPort);
            QMessageBox::critical(NULL, tr("Network Error"), tr("Error to listen on port %1. May it be occupied by another program?").arg(info->dstPort));
            return;
        } else {
            qDebug("udp server bind to port %d", info->dstPort);
        }
        connect(socket, SIGNAL(readyRead()), this, SLOT(readPendingData()));
        qDebug("udp server made");
    }
        break;
    case CONNECTION_TCP_SERVER:
    {
        socket = new QTcpServer(this);
        qDebug("making tcp server");
        QTcpServer *server = qobject_cast<QTcpServer *>(socket);
        bool success = server->listen(QHostAddress::Any, info->dstPort);
        if (!success) {
            qCritical("failed to listen on port %d", info->dstPort);
            QMessageBox::critical(NULL, tr("Network Error"), tr("Error to listen on port %1. May it be occupied by another program?").arg(info->dstPort));
            return;
        } else {
            qDebug("tcp server listen on port %d", info->dstPort);
        }
        connect(server, SIGNAL(newConnection()), this, SLOT(processConnection()));
        qDebug("tcp server made");
    }
        break;
    default:
        break;
    }
}

void Connection::linkDown()
{
    if (info->conType != CONNECTION_TCP_SERVER)
        qobject_cast<QIODevice *>(socket)->close();
    else
        qobject_cast<QTcpServer *>(socket)->close();
    qDebug("connection closed");
}

void Connection::setConnectionType(ConnectionType type) {
    info->conType = type;
}

ConnectionType Connection::connectionType() const
{
    return info->conType;
}

QUuid Connection::id() const
{
    return uuid;
}

bool Connection::isConnected() const
{
    return connected;
}

void Connection::send(const QByteArray &data)
{
    switch(info->conType) {
    case CONNECTION_UDP_CLIENT:
    case CONNECTION_UDP_SERVER:
    {
        qint64 sendedBytes = ((QUdpSocket *)socket)->writeDatagram(data, info->dstAddress, info->dstPort);
        if (sendedBytes != -1) {
            qDebug("%lld bytes sended", sendedBytes);
            emit sendedDataChanged(QString(data));
        } else {
            qDebug("data send error");
        }
    }
    break;
    case CONNECTION_TCP_CLIENT:
    {
        qint64 sendedBytes = qobject_cast<QTcpSocket *>(socket)->write(data);
        if (sendedBytes != -1) {
            qDebug("%lld bytes sended", sendedBytes);
            emit sendedDataChanged(QString(data));
        } else {
            qDebug("data send error");
        }
    }
    break;
    default:
        break;
    }
}

void Connection::readPendingData()
{
    switch(info->conType) {
    case CONNECTION_UDP_CLIENT:
    case CONNECTION_UDP_SERVER:
    {
        QUdpSocket *udpSocket = (QUdpSocket *)socket;
        while(udpSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(udpSocket->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            if (info->conType == CONNECTION_UDP_SERVER && senderPort == info->dstPort) {
                //nothing to do here
            } else {
                qDebug("data received");
                emit dataReceived(QString(datagram));
            }
        }
    }
    break;

    case CONNECTION_TCP_CLIENT:
        qDebug("data received");
        emit dataReceived(QString(qobject_cast<QTcpSocket *>(socket)->readAll()));
    break;

    case CONNECTION_TCP_SERVER:
        qDebug("data received");
        emit dataReceived(QString("%1::::%2::::%3").arg(connection->peerAddress().toString()).arg(connection->peerPort()).arg(QString(connection->readAll())));
    break;
    }
}

void Connection::processConnection()
{
    QTcpServer *server = qobject_cast<QTcpServer *>(socket);
    connection = server->nextPendingConnection();
    connect(connection, SIGNAL(readyRead()), this, SLOT(readPendingData()));
    connect(connection, SIGNAL(disconnected()), this, SLOT(processConnectionClose()));
    qDebug("new connection established. ip: %s", qPrintable(connection->peerAddress().toString()));
    emit newConnectionEstablished(connection->peerAddress().toString());
}

void Connection::processConnectionClose()
{
    qDebug("connection closed. ip: %s", qPrintable(connection->peerAddress().toString()));
    emit connectionClosed(connection->peerAddress().toString());
}

void Connection::setConnectionInfo(ConnectionInfo *conInfo)
{
    info = conInfo;
}

Connection::~Connection()
{
    if (info) {
        delete info;
        info = NULL;
    }
}

ConnectionInfo *Connection::connectionInfo() const
{
    return info;
}

quint16 Connection::sourcePort() const
{
    return info->srcPort;
}

quint16 Connection::serverPort() const
{
    return info->dstPort;
}
