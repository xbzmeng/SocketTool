#include "connectiondisplaymodel.h"
#include "connectionstoragemanager.h"

ConnectionDisplayModel::ConnectionDisplayModel(QObject *parent) :
    QAbstractListModel(parent)
{
    ConnectionStorageManager* manager = ConnectionStorageManager::manager();
    connect(manager, SIGNAL(newConnection(ConnectionInfo*)), this, SLOT(newConnection()));
    connect(manager, SIGNAL(moveUp(int)), this, SLOT(moveUp(int)));
    connect(manager, SIGNAL(moveDown(int)), this, SLOT(moveDown(int)));
    connect(manager, SIGNAL(connectionDeleted(int)), this, SLOT(deleteItem(int)), Qt::DirectConnection);
}

int ConnectionDisplayModel::rowCount(const QModelIndex &) const
{
    return ConnectionStorageManager::manager()->connectionCount();
}

QVariant ConnectionDisplayModel::data(const QModelIndex &, int ) const
{
    return QVariant();
}

void ConnectionDisplayModel::newConnection()
{
    int itemCount = ConnectionStorageManager::manager()->connectionCount();
    beginInsertRows(QModelIndex(), itemCount - 1, itemCount - 1);
    endInsertRows();
}

void ConnectionDisplayModel::moveUp(int index)
{
    emit dataChanged(createIndex(index - 1, 0), createIndex(index, 0));
}

void ConnectionDisplayModel::moveDown(int index)
{
    emit dataChanged(createIndex(index, 0), createIndex(index + 1, 0));
}

void ConnectionDisplayModel::deleteItem(int index)
{
        beginRemoveRows(QModelIndex(), index, index);
        removeRow(index);
        endRemoveRows();
}
