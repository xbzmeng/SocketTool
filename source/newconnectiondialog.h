#ifndef NEWCONNECTIONDIALOG_H
#define NEWCONNECTIONDIALOG_H

#include <QDialog>
#include "connection.h"
#include <QUuid>

namespace Ui {
class NewConnectionDialog;
}

class NewConnectionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewConnectionDialog(QWidget *parent = 0);
    ~NewConnectionDialog();
    ConnectionType connectionType() const;

    virtual void accept();
    virtual void reject();

    QUuid connectionId() const;

private slots:
    void connectionTypeChanged(int);
    void toggleBroadcast(bool);
    void saveConnection();
    void toggleSaveBtn(int);
    void moveUp();
    void moveDown();
    void deleteConnection();
private:
    void updateIpBox(bool broadcast);
    ConnectionInfo* getConnectionInfo();
    Ui::NewConnectionDialog *ui;
    ConnectionType mConnectionType;
    QUuid conId;
    QList<ConnectionInfo*> infos;

    QAction *moveUpAction;
    QAction *moveDownAction;
    QAction *deleteAction;
};

#endif // NEWCONNECTIONDIALOG_H
