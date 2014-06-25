#ifndef CONNECTIONSTORAGEMANAGER_H
#define CONNECTIONSTORAGEMANAGER_H

#include <QObject>
#include "connectioninfo.h"

class ConnectionStorageManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ConnectionStorageManager)
public:
    static ConnectionStorageManager* manager();
    ~ConnectionStorageManager();
    void addConnectionInfo(ConnectionInfo *);
    void removeConnectionInfo(ConnectionInfo *);
    int connectionCount() const;
    ConnectionInfo *at(int index) const;
    void saveConnections();
    void loadConnections();
    void moveUpConnection(int);
    void moveDownConnection(int);
    void deleteConnection(int);
private:
    ConnectionStorageManager(QObject *parent = 0);
signals:
    void newConnection(ConnectionInfo *);
    void moveUp(int);
    void moveDown(int);
    void connectionDeleted(int);

public slots:
private:
    QString configPath() const;
    static ConnectionStorageManager *globalManager;
    QList<ConnectionInfo *> infos;
};

#endif // CONNECTIONSTORAGEMANAGER_H
