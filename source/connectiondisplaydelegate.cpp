#include "connectiondisplaydelegate.h"
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include "connectionstoragemanager.h"

ConnectionDisplayDelegate::ConnectionDisplayDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QString ConnectionDisplayDelegate::connectionInfoText(ConnectionInfo *info)
{
    QString conInfo;
    switch (info->conType) {
    case CONNECTION_UDP_CLIENT:
        conInfo = tr("<font color=\"green\" size=\"+1\"><strong>UDP Client</strong></font>"
                  "<p style=\"margin-left:10px;margin-top:-10px;\">Destination Address: %1<br/>"
                  "Destination Port: %2<br/>"
                  "Source Port: %3</p>")
                .arg(info->dstAddress.toString())
                .arg(info->dstPort)
                .arg(info->randomSrcPort ? tr("Random") : QString::number(info->srcPort));
    break;
    case CONNECTION_UDP_SERVER:
        conInfo = tr("<font color=\"green\" size=\"+1\"><strong>UDP Server</strong></font>"
                  "<p style=\"margin-left:10px;margin-top:-10px;\">Listen on: %1</p>")
                .arg(info->dstPort);
    break;
    case CONNECTION_TCP_SERVER:
        conInfo = tr("<font color=\"green\" size=\"+1\"><strong>TCP Server</strong></font>"
                  "<p style=\"margin-left:10px;margin-top:-10px;\">Listen on: %1</p>")
                .arg(info->dstPort);
    break;
    case CONNECTION_TCP_CLIENT:
        conInfo = tr("<font color=\"green\" size=\"+1\"><strong>TCP Client</strong></font>"
                  "<p style=\"margin-left:10px;margin-top:-10px;\">Destination Address: %1<br/>"
                  "Destination Port: %2</p>")
                .arg(info->dstAddress.toString())
                .arg(info->dstPort);
    break;
    }
    return conInfo;
}

void ConnectionDisplayDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget ? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    ConnectionInfo *info = ConnectionStorageManager::manager()->at(index.row());
    doc.setHtml(connectionInfoText(info));
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();

}

QSize ConnectionDisplayDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    ConnectionInfo *info = ConnectionStorageManager::manager()->at(index.row());
    QString conInfo = this->connectionInfoText(info);
    doc.setHtml(conInfo);
    doc.setTextWidth(optionV4.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}
