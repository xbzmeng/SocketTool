#ifndef IPCTRL_H
#define IPCTRL_H

#include <QFrame>
#include <QLineEdit>
#include <QIntValidator>
#include "stdint.h"
#include <QHBoxLayout>
#include <QFont>
#include <QLabel>
#include <QKeyEvent>


class IPCtrl : public QFrame
{
    Q_OBJECT

public:
    IPCtrl(QWidget *parent = 0);
    ~IPCtrl();

    virtual bool eventFilter( QObject *obj, QEvent *event );
	QString ipAddress() const;
    void setIpAddress(const QString &address, bool broadcast = false);

public slots:
    void slotTextChanged( QLineEdit* pEdit );

signals:
    void signalTextChanged( QLineEdit* pEdit );

private:
    enum
    {
        QTUTL_IP_SIZE   = 4,
        MAX_DIGITS      = 3 
    };

    QLineEdit *(m_pLineEdit[QTUTL_IP_SIZE]);
    void MoveNextLineEdit (int i);
    void MovePrevLineEdit (int i);
};

#endif
