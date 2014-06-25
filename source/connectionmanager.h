#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QMap>
#include <QUuid>
#include "connection.h"

class ConnectionManager : QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ConnectionManager)
public:
    static ConnectionManager* manager();
    void addConnection(Connection *);
    bool removeConnection(const QUuid &);
    Connection *findConnection(const QUuid &uuid) const;
private:
    ConnectionManager(QObject *parent = NULL);
private:
    QMap<QUuid, Connection*> connections;
    static ConnectionManager *globalManager;

};

#endif // CONNECTIONMANAGER_H
