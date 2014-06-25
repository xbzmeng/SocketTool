#include "newconnectiondialog.h"
#include "ui_newconnectiondialog.h"
#include "connectionmanager.h"
#include "connectionstoragemanager.h"
#include "connectiondisplaydelegate.h"
#include <QtNetwork>
#include <QToolBar>
#include <QMessageBox>
#include "connectiondisplaymodel.h"

NewConnectionDialog::NewConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewConnectionDialog)
{
    ui->setupUi(this);

    mConnectionType = CONNECTION_UDP_CLIENT;

    connect(ui->connectionTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(connectionTypeChanged(int)));

    ui->connectionTypeBox->setCurrentIndex(mConnectionType);
    connectionTypeChanged(mConnectionType);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->broadcastBox, SIGNAL(clicked(bool)), this, SLOT(toggleBroadcast(bool)));

    updateIpBox(false);
    connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveConnection()));
    ui->connectionList->setItemDelegate(new ConnectionDisplayDelegate);
    ui->connectionList->setModel(new ConnectionDisplayModel);
    connect(ui->sessionTypeList, SIGNAL(currentRowChanged(int)), this, SLOT(toggleSaveBtn(int)));


    QToolBar *toolbar = new QToolBar;
    moveUpAction = new QAction(tr("Move Up"), toolbar);
    moveUpAction->setIcon(QIcon(":/resources/icons/arrow_up.png"));
    toolbar->addAction(moveUpAction);
    moveDownAction = new QAction(tr("Move Down"), toolbar);
    moveDownAction->setIcon(QIcon(":/resources/icons/arrow_down.png"));
    toolbar->addAction(moveDownAction);
    QLabel *label = new QLabel;
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(label);
    deleteAction = new QAction(tr("Delete"), toolbar);
    deleteAction->setIcon(QIcon(":/resources/icons/delete.png"));
    toolbar->addAction(deleteAction);
    toolbar->setStyleSheet("QToolBar{border:0}");
    ui->stackedWidget->widget(1)->layout()->addWidget(toolbar);
    connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUp()));
    connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDown()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteConnection()));

    connect(ui->connectionList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));

}


void NewConnectionDialog::moveUp()
{
    QModelIndex index = ui->connectionList->currentIndex();
    if (index.isValid()) {
        if (index.row() > 0) {
            ConnectionStorageManager *manager = ConnectionStorageManager::manager();
            manager->moveUpConnection(index.row());
            ui->connectionList->setCurrentIndex(ui->connectionList->model()->index(index.row() - 1, 0));
        }
    }
}

void NewConnectionDialog::moveDown()
{
    QModelIndex index = ui->connectionList->currentIndex();
    if (index.isValid()) {
        if (index.row() < ui->connectionList->model()->rowCount() - 1) {
            ConnectionStorageManager::manager()->moveDownConnection(index.row());

            ui->connectionList->setCurrentIndex(ui->connectionList->model()->index(index.row() + 1, 0));
        }
    }
}

void NewConnectionDialog::deleteConnection()
{
    QModelIndex index = ui->connectionList->currentIndex();
    if (index.isValid()) {
        ConnectionStorageManager::manager()->deleteConnection(index.row());
    }
}

NewConnectionDialog::~NewConnectionDialog()
{
    delete ui;
}

void NewConnectionDialog::toggleSaveBtn(int index)
{
    ui->saveBtn->setEnabled(!index);
}

void NewConnectionDialog::connectionTypeChanged(int type)
{
    bool isClient = type == CONNECTION_UDP_CLIENT || type == CONNECTION_TCP_CLIENT;
    ui->dstIpLabel->setVisible(isClient);
    ui->dstIpBox->setVisible(isClient);
    ui->dstPortLabel->setVisible(isClient);
    ui->dstPortSpin->setVisible(isClient);
    ui->srcPortLabel->setVisible(type == CONNECTION_UDP_CLIENT);
    ui->srcPortSpin->setVisible(type == CONNECTION_UDP_CLIENT);
    ui->serverPortLabel->setVisible(!isClient);
    ui->serverPortSpin->setVisible(!isClient);
    mConnectionType = (ConnectionType)type;
    ui->broadcastBox->setVisible(type == CONNECTION_UDP_CLIENT);
}


ConnectionType NewConnectionDialog::connectionType() const
{
    return mConnectionType;
}

ConnectionInfo* NewConnectionDialog::getConnectionInfo()
{
    ConnectionInfo *info = NULL;
    if (ui->sessionTypeList->currentRow() == 0) {
        if (ui->connectionTypeBox->currentIndex() == CONNECTION_UDP_CLIENT ||
                ui->connectionTypeBox->currentIndex() == CONNECTION_TCP_CLIENT) {
            QHostAddress serverAddress(ui->dstIpBox->ipAddress());
            if (serverAddress.isNull()) {
                qCritical("illegal server ip address entered");
                QMessageBox::critical(this, tr("Server IP Error"), tr("Illegal destination IP address."));
                return NULL;
            }
        }
        info = new ConnectionInfo();
        info->conType = (ConnectionType)ui->connectionTypeBox->currentIndex();
        switch(info->conType) {
        case CONNECTION_UDP_CLIENT:
        case CONNECTION_TCP_CLIENT:
        {
            QHostAddress serverAddress(ui->dstIpBox->ipAddress());
            info->dstAddress = serverAddress;
            info->dstPort = ui->dstPortSpin->value();
            info->srcAddress = QHostAddress(QHostAddress::LocalHost);
            info->srcPort = ui->srcPortSpin->value();
            info->randomSrcPort = info->srcPort == 0;
        }
        break;
        case CONNECTION_UDP_SERVER:
            info->dstAddress = QHostAddress(QHostAddress::Broadcast);
            info->dstPort = ui->serverPortSpin->value();
        break;
        case CONNECTION_TCP_SERVER:
            info->dstAddress = QHostAddress(QHostAddress::LocalHost);
            info->dstPort = ui->serverPortSpin->value();
        break;
        }
    } else if (ui->sessionTypeList->currentRow() == 1) {
        if (ui->connectionList->model()->rowCount() == 0)
            return NULL;

        if (!ui->connectionList->currentIndex().isValid()) {
            return NULL;
        }

        int row = ui->connectionList->currentIndex().row();
        ConnectionInfo *conInfo = ConnectionStorageManager::manager()->at(row);
        info = new ConnectionInfo();
        info->dstAddress = conInfo->dstAddress;
        info->dstPort = conInfo->dstPort;
        info->srcAddress = conInfo->srcAddress;
        info->srcPort = conInfo->srcPort;
        info->conType = conInfo->conType;
        info->randomSrcPort = conInfo->randomSrcPort;
        mConnectionType = info->conType;
    }

    return info;
}

void NewConnectionDialog::accept()
{
    ConnectionInfo *info = getConnectionInfo();
    if (info) {
        ConnectionManager *manager = ConnectionManager::manager();
        Connection *connection = new Connection();
        connection->setConnectionInfo(info);
        manager->addConnection(connection);
        conId = connection->id();
    } else {
        QMessageBox::critical(this, tr("Setting Error"), tr("Please set connection to be established."));
        return;
    }

    QDialog::accept();
}

void NewConnectionDialog::reject()
{
    QDialog::reject();
}

QUuid NewConnectionDialog::connectionId() const
{
    return conId;
}

void NewConnectionDialog::toggleBroadcast(bool broadcast)
{
    if (broadcast) {
        updateIpBox(true);
    }
}

void NewConnectionDialog::updateIpBox(bool broadcast)
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QString ipAddress = ui->dstIpBox->ipAddress();
    if (QHostAddress(ipAddress).isNull()) {
        for(int nIter=0; nIter<list.count(); nIter++) {
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ) {
                ipAddress = list[nIter].toString();
                break;
            }
        }
    }

    ui->dstIpBox->setIpAddress(ipAddress, broadcast);
}

void NewConnectionDialog::saveConnection()
{
    ConnectionInfo *info = getConnectionInfo();
    if (info) {
        ConnectionStorageManager *manager = ConnectionStorageManager::manager();
        manager->addConnectionInfo(info);
    }
}
