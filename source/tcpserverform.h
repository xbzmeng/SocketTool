#ifndef TCPSERVERFORM_H
#define TCPSERVERFORM_H

#include <QAbstractSocket>
#include "connectionform.h"

class QToolButton;
class QCheckBox;
class QPushButton;

class TcpServerForm : public ConnectionForm
{
    Q_OBJECT
public:
    explicit TcpServerForm(QWidget *parent = 0);
    void setConnectionId(const QUuid &);
private slots:
    void connectToHost();
    void networkStateChanged(QAbstractSocket::SocketState socketState);
    void newConnection(const QString &);
    void connectionClosed(const QString &);
    void dataReceived(const QString &);
private:
    void createControls();
    bool isConnected;
};

#endif // TCPSERVERFORM_H
