#include "connectionstoragemanager.h"
#include <QtGui>

ConnectionStorageManager *ConnectionStorageManager::globalManager = NULL;

ConnectionStorageManager::ConnectionStorageManager(QObject *parent) :
    QObject(parent)
{
}

ConnectionStorageManager::~ConnectionStorageManager()
{
    while(infos.length()) {
        delete infos.takeLast();
    }
}

ConnectionStorageManager *ConnectionStorageManager::manager()
{
    if (globalManager == NULL) {
        qDebug("connection storage manager created");
        globalManager = new ConnectionStorageManager();
    }
    return globalManager;
}

void ConnectionStorageManager::addConnectionInfo(ConnectionInfo *info)
{
    for (int i = 0; i < infos.length(); ++i)
        if (*infos[i] == *info)
            return;
    infos << info;
    qDebug("new connection information added");
    emit newConnection(info);
}


void ConnectionStorageManager::removeConnectionInfo(ConnectionInfo *info)
{
    if (infos.contains(info)) {
        infos.removeAll(info);
        qDebug("connection information deleted");
        delete info;
    }
}

int ConnectionStorageManager::connectionCount() const
{
    return infos.length();
}

ConnectionInfo* ConnectionStorageManager::at(int index) const
{
    if (index < infos.length())
        return infos[index];
    return NULL;
}

QString ConnectionStorageManager::configPath() const
{
    QString config;
#if QT_VERSION >= 0x050000
    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
    if (!locations.isEmpty()) config = locations[0];
#endif

    if (config.isEmpty()) {
        QString configPath;
#ifdef Q_WS_WIN
        configPath = QDir::homePath() + "/Application Data/";
#else
        configPath = QDir::homePath() + "/.config/SocketTool/";
#endif
        if (!QDir(configPath).exists())
            QDir().mkpath(configPath);
        config = configPath + "/config.dat";
    }
    return config;
}

void ConnectionStorageManager::saveConnections()
{
    if (!infos.length()) return;
    QString config = configPath();
    QFile file(config);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical("failed to save connections");
        return;
    }
    QDataStream out(&file);
    out << infos.length();
    for (int i = 0; i < infos.length(); ++i)
        out << *(infos[i]);
    file.close();
    qDebug("connection saved to %s", qPrintable(config));
}

void ConnectionStorageManager::loadConnections()
{
    QString config = configPath();
    if (QFile::exists(config)) {
        qDebug("loading saved connections from %s", qPrintable(config));
    } else {
        qDebug("found no saved connections");
        return;
    }

    QFile file(config);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical("failed to load saved connections file");
        return;
    }

    QDataStream in(&file);
    int length;
    in >> length;
    qDebug("found %d saved connections", length);
    for (int i = 0; i < length; ++i) {
        ConnectionInfo info;
        in >> info;
        ConnectionInfo *conInfo = new ConnectionInfo();
        conInfo->conType = info.conType;
        conInfo->dstAddress = info.dstAddress;
        conInfo->dstPort = info.dstPort;
        conInfo->srcAddress = info.srcAddress;
        conInfo->srcPort = info.srcPort;
        conInfo->randomSrcPort = info.randomSrcPort;
        addConnectionInfo(conInfo);
    }
}

void ConnectionStorageManager::moveUpConnection(int index)
{
    if (index > 0) {
        infos.swap(index - 1, index);
        qDebug("move up");
        emit moveUp(index);
    }
}

void ConnectionStorageManager::moveDownConnection(int index)
{
    if (index < infos.length() - 1) {
        infos.swap(index, index + 1);
        qDebug("move down");
        emit moveDown(index);
    }
}

void ConnectionStorageManager::deleteConnection(int index)
{
    if (index >= 0 && index < infos.length()) {
        emit connectionDeleted(index);
        qDebug("delete connection");
        delete infos.takeAt(index);
    }
}
