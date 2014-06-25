#ifndef CONNECTIONDISPLAYDELEGATE_H
#define CONNECTIONDISPLAYDELEGATE_H

#include <QStyledItemDelegate>
#include "connectioninfo.h"

class ConnectionDisplayDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ConnectionDisplayDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:

public slots:
private:
    static QString connectionInfoText(ConnectionInfo *);

};

#endif // CONNECTIONDISPLAYDELEGATE_H
