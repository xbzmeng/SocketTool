#include "connectionmanager.h"

ConnectionManager *ConnectionManager::globalManager = NULL;

ConnectionManager::ConnectionManager(QObject *parent)
    :QObject(parent)
{
}

ConnectionManager* ConnectionManager::manager()
{
    if (globalManager == NULL) {
        qDebug("connection manager created.");
        globalManager = new ConnectionManager();
    }
    return globalManager;
}

void ConnectionManager::addConnection(Connection *info)
{
    if (!connections.contains(info->id())) {
        connections[info->id()] = info;
        info->setParent(this);
        qDebug("connection %s added to connection manager", qPrintable(info->id().toString()));
    }
}

Connection *ConnectionManager::findConnection(const QUuid &uuid) const
{
    if (connections.contains(uuid)) {
        return connections[uuid];
    }
    return NULL;
}

bool ConnectionManager::removeConnection(const QUuid &uuid)
{
    Connection *info = findConnection(uuid);
    if (info == NULL) return false;
    connections.remove(uuid);
    qDebug("connection %s removed from connection manager", qPrintable(uuid.toString()));
    info->linkDown();
    qDebug("connection deleted");
    delete info;
    return true;
}
