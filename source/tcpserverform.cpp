#include "tcpserverform.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QTextEdit>
#include "connectionmanager.h"

TcpServerForm::TcpServerForm(QWidget *parent) :
    ConnectionForm(parent)
{
    isConnected = false;
    packetMode = true;
    createControls();
}

void TcpServerForm::createControls()
{
    ConnectionForm::createControls();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(utilityBar);

    mainLayout->addWidget(receiveBox);

}

void TcpServerForm::setConnectionId(const QUuid &uuid)
{
    conId = uuid;
    Connection *conInfo = ConnectionManager::manager()->findConnection(conId);

    if (conInfo != NULL) {
        connect(conInfo, SIGNAL(sendedDataChanged(const QString&)), this, SLOT(dataSended(const QString&)));
        connect(conInfo, SIGNAL(dataReceived(const QString&)), this, SLOT(dataReceived(const QString&)));
        connect(conInfo, SIGNAL(newConnectionEstablished(const QString&)), this, SLOT(newConnection(const QString&)));
        connect(conInfo, SIGNAL(connectionClosed(QString)), this, SLOT(connectionClosed(const QString&)));
    }
}

void TcpServerForm::connectToHost()
{
    Connection *conInfo = ConnectionManager::manager()->findConnection(conId);
    conInfo->setTcpConnected(!isConnected);
}

void TcpServerForm::networkStateChanged(QAbstractSocket::SocketState socketState)
{
    switch(socketState) {
    case QAbstractSocket::UnconnectedState:
        isConnected = false;
        break;
    case QAbstractSocket::ConnectedState:
        isConnected = true;
        break;
    default:
        break;
    }
}

void TcpServerForm::newConnection(const QString &address)
{
    receiveBox->append(tr("%1 connected.").arg(address));
}

void TcpServerForm::connectionClosed(const QString &address)
{
    receiveBox->append(tr("%1 closed.").arg(address));
}

void TcpServerForm::dataReceived(const QString &data)
{
    QTextCursor textCursor = receiveBox->textCursor();
    textCursor.movePosition(QTextCursor::End);
    receiveBox->setTextCursor(textCursor);
    receiveBox->ensureCursorVisible();

    if (packetMode) {
        QStringList dats = data.split("::::");
        receiveBox->append(tr("<font color=\"blue\">Received%1%2: </font>%3").arg(displayIp ? tr(" from ") + dats[0] : "").arg(displayIp && displayPort ? ":" + dats[1] : "").arg(dats[2]));
    } else {
        receiveBox->insertPlainText(tr("%1").arg(data));
    }
}
