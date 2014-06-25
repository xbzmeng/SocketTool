#ifndef CONNECTIONDISPLAYMODEL_H
#define CONNECTIONDISPLAYMODEL_H

#include <QAbstractListModel>
#include "connectioninfo.h"

class ConnectionDisplayModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ConnectionDisplayModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
signals:

public slots:
    void newConnection();
    void moveUp(int);
    void moveDown(int);
    void deleteItem(int);
};

#endif // CONNECTIONDISPLAYMODEL_H
