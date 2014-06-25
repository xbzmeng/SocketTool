#ifndef CLIENTFORM_H
#define CLIENTFORM_H

#include "connectionform.h"
#include <QAbstractSocket>

class QToolButton;
class QCheckBox;
class QPushButton;

class ClientForm : public ConnectionForm
{
    Q_OBJECT
public:
    explicit ClientForm(QWidget *parent = 0, bool udp = true);
    void setConnectionId(const QUuid &);
    void setTextFont(const QFont &);
signals:

public slots:
    void send();
    void sendFormatChanged();
    void enterProcessChanged();
    void connectToHost();
    void networkStateChanged(QAbstractSocket::SocketState socketState);
protected:
    bool eventFilter(QObject *, QEvent *);
    void createControls();
    QTextEdit *senderBox;
    QToolButton *sendButton;
    bool hexMode;
    QCheckBox *hexFormatBox;
    bool enterSend;
    QAction *ctrlEnterSendAction;
    QAction *enterSendAction;
    bool isUdpForm;
    bool isConnected;
    QPushButton *connectButton;
};

#endif // CLIENTFORM_H
