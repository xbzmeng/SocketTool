#include "clientform.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QSplitter>
#include <QToolBar>
#include <QPushButton>
#include <QCheckBox>
#include <QToolButton>
#include <QMenu>
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include "connectionmanager.h"

ClientForm::ClientForm(QWidget *parent, bool udp) :
    ConnectionForm(parent)
{
    isConnected = false;
    hexMode = false;
    enterSend = true;
    isUdpForm = udp;
    createControls();
}

void ClientForm::setTextFont(const QFont &font)
{
    receiveBox->setFont(font);
    senderBox->setFont(font);
}

void ClientForm::createControls()
{
    ConnectionForm::createControls();


    QVBoxLayout *layout = new QVBoxLayout(this);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    layout->addWidget(splitter);
    splitter->addWidget(receiveBox);
    splitter->setStretchFactor(0, 3);


    QWidget *w = new QWidget(splitter);
    splitter->addWidget(w);
    splitter->setStretchFactor(1, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout(w);
    mainLayout->setMargin(0);
    mainLayout->addWidget(utilityBar);

    senderBox = new QTextEdit;
    senderBox->setFocusPolicy(Qt::StrongFocus);
    senderBox->installEventFilter(this);
    mainLayout->addWidget(senderBox);

    QHBoxLayout *bottomLayout = new QHBoxLayout;

    if (isUdpForm) {
        connectButton = NULL;
    } else {
        connectButton = new QPushButton(tr("&Connect"));
        bottomLayout->addWidget(connectButton);
    }
    hexFormatBox = new QCheckBox(tr("Send as Hex"));
    connect(hexFormatBox, SIGNAL(clicked()), this, SLOT(sendFormatChanged()));
    bottomLayout->addWidget(hexFormatBox);

    bottomLayout->addStretch(1);

    sendButton = new QToolButton;
    sendButton->setText(tr("&Send"));
    bottomLayout->addWidget(sendButton);
    sendButton->setPopupMode(QToolButton::MenuButtonPopup);
    sendButton->setStyleSheet("QToolButton {padding: 3px;padding-right:20px;padding-left:10px;}");

    QMenu *menu = new QMenu;
    enterSendAction = new QAction(tr("Press Enter to Send"), this);
    ctrlEnterSendAction = new QAction(tr("Press Ctrl+Enter to Send"), this);
    QActionGroup *group = new QActionGroup(this);
    enterSendAction->setCheckable(true);
    ctrlEnterSendAction->setCheckable(true);
    enterSendAction->setActionGroup(group);
    ctrlEnterSendAction->setActionGroup(group);
    menu->addAction(enterSendAction);
    menu->addAction(ctrlEnterSendAction);
    sendButton->setMenu(menu);
    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(enterProcessChanged()));
    enterSendAction->setChecked(true);

    mainLayout->addLayout(bottomLayout);

    connect(sendButton, SIGNAL(clicked()), this, SLOT(send()));
    if (!isUdpForm) {
        connect(connectButton, SIGNAL(clicked()), this, SLOT(connectToHost()));
    }
}

void ClientForm::setConnectionId(const QUuid &uuid) {
    conId = uuid;
    Connection *conInfo = ConnectionManager::manager()->findConnection(conId);
    if (conInfo) {
        connect(conInfo, SIGNAL(sendedDataChanged(const QString&)), this, SLOT(dataSended(const QString&)));
        connect(conInfo, SIGNAL(dataReceived(const QString&)), this, SLOT(dataReceived(const QString&)));
        if (!isUdpForm) {
            connect(conInfo, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(networkStateChanged(QAbstractSocket::SocketState)));
        }
    }
}

void ClientForm::send()
{
    if (senderBox->toPlainText().isEmpty()) {
        return;
    }
    if (!isUdpForm && !isConnected) {
        qCritical("tcp connection is not established");
        QMessageBox::critical(this, tr("Connection Error"), tr("Connection is not established. Please connect to the server firstly."));
        return;
    }

    Connection *conInfo = ConnectionManager::manager()->findConnection(conId);
    if (conInfo != NULL) {
        QString text = senderBox->toPlainText();
        if (!hexFormatBox->isChecked()) {
            conInfo->send(text.toLatin1());
        } else {
            qDebug("changing to hex format");
            int offset = text.length() % 2;
            QByteArray data;
            if (offset) data.append(text.left(1).toInt(NULL, 16));
            for (int i = offset; i < text.length(); i += 2) {
                data.append(text.mid(i, 2).toInt(NULL, 16));
            }
            conInfo->send(data);
        }
        qDebug("text sended");
        senderBox->clear();
        senderBox->setFocus();
    }
}

bool ClientForm::eventFilter(QObject *obj, QEvent *evt)
{
    if (obj == senderBox && evt->type() == QEvent::KeyPress) {
        QKeyEvent *event = static_cast<QKeyEvent *>(evt);
        int key = event->key();
        if (key == Qt::Key_Enter || key == Qt::Key_Return) {
            if (enterSend || (!enterSend && (event->modifiers() & Qt::ControlModifier))) {
                qDebug("keyboard shortcut pressed");
                send();
                return true;
            }
        }

        if (hexMode) {
            if((key >= '0' && key <= '9') ||
               (key >= 'a' && key <= 'f') ||
               (key >= 'A' && key <= 'F')) {

            } else {
                qDebug("illegal key pressed");
                QApplication::beep();
                return true;
            }
        }

    }
    return ConnectionForm::eventFilter(obj, evt);
}

void ClientForm::connectToHost()
{
    if (!isUdpForm) {
        Connection *conInfo = ConnectionManager::manager()->findConnection(conId);
        if (conInfo) {
            conInfo->setTcpConnected(!isConnected);
        }
    }
}

void ClientForm::networkStateChanged(QAbstractSocket::SocketState socketState)
{
    if (isUdpForm) return;

    switch(socketState) {
    case QAbstractSocket::UnconnectedState:
        isConnected = false;
        connectButton->setText(tr("&Connect"));
        qDebug("connection closed");
        break;
    case QAbstractSocket::ConnectedState:
        isConnected = true;
        connectButton->setText(tr("&Disconnect"));
        qDebug("connected");
        break;
    default:
        break;
    }
}

void ClientForm::sendFormatChanged()
{
    qDebug("send format changed");
    hexMode = hexFormatBox->isChecked();
    senderBox->clear();
}

void ClientForm::enterProcessChanged()
{
    if (enterSendAction->isChecked()) {
        qDebug("use enter to send");
        enterSend = true;
    } else {
        qDebug("use ctrl+enter to send");
        enterSend = false;
    }
}
