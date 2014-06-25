#ifndef CONNECTIONFORM_H
#define CONNECTIONFORM_H

#include <QWidget>
#include <QUuid>

class QTextEdit;
class QToolBar;
class QVBoxLayout;

class ConnectionForm : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionForm(QWidget *parent = 0);
    virtual ~ConnectionForm() {}
    virtual void setConnectionId(const QUuid &) = 0;
    QUuid connectionId() const;
    void setPacketMode(bool packet);
    virtual void setTextFont(const QFont &);
    virtual void setBoxEditable(bool);
    virtual void setDisplayIP(bool);
    virtual void setDisplayPort(bool);
signals:

private slots:
    void dataSended(const QString &);
    virtual void dataReceived(const QString &);
    void clearText();
    void exportResult();
    void changeDisplayMode(int mode);
protected:
    void createControls();
    QTextEdit *receiveBox;
    QToolBar *utilityBar;
    QUuid conId;
    bool packetMode;
    bool isConnected;
    bool displayIp;
    bool displayPort;
};

#endif // CONNECTIONFORM_H
