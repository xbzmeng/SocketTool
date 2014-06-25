#include <QtGui>
#include "infodockwidget.h"
#include "connectionmanager.h"
#include <QtNetwork>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>

InfoDockWidget::InfoDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    setWindowTitle(tr("Connection Information"));
    createControls();
    setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
}

void InfoDockWidget::createControls()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);

    infoLabel = new QLabel();
    infoLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(infoLabel);

    list = new QListWidget();
    list->hide();
    layout->addWidget(list);

    list->setStyleSheet("QListWidget {border:0;background:transparent;}");

    setWidget(w);

}

void InfoDockWidget::setConnectionId(const QUuid &uuid)
{
    conId = uuid;
    updateInformation();
}

void InfoDockWidget::updateInformation()
{
    Connection *connection = ConnectionManager::manager()->findConnection(conId);
    if (connection == NULL) return;

    ConnectionInfo *conInfo = connection->connectionInfo();
    QString info;




    if (conInfo != NULL) {
        QString localHost = "127.0.0.1";
        if (conInfo->srcAddress.toString() == localHost) {
            QList<QHostAddress> list = QNetworkInterface::allAddresses();
            for(int nIter=0; nIter<list.count(); nIter++) {
                if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ) {
                    localHost = list[nIter].toString();
                    break;
                }
            }
        } else {
            localHost = conInfo->srcAddress.toString();
        }



        switch(conInfo->conType) {
        case CONNECTION_UDP_CLIENT:
            info = tr("Destination IP: %1   \nDestination Port: %2\nSource IP: %3   \nSource Port: %4")
                            .arg(conInfo->dstAddress.toString())
                            .arg(conInfo->dstPort)
                            .arg(localHost)
                            .arg(conInfo->srcPort);
            list->hide();
            break;
        case CONNECTION_TCP_CLIENT:
            info = tr("Destination IP: %1   \nDestination Port: %2\nSource IP: %3")
                            .arg(conInfo->dstAddress.toString())
                            .arg(conInfo->dstPort)
                            .arg(localHost);
            list->hide();
        break;
        case CONNECTION_TCP_SERVER:
        case CONNECTION_UDP_SERVER:
            info = tr("Listen on port %1.  ").arg(conInfo->dstPort);
            if (conInfo->conType == CONNECTION_TCP_SERVER)
                list->hide();
            break;
        }

        infoLabel->setText(info);
    }
}

void InfoDockWidget::clearText()
{
    infoLabel->setText("");
}
