#ifndef UDPCLIENTINFODOCKWIDGET_H
#define UDPCLIENTINFODOCKWIDGET_H

#include <QDockWidget>
#include <QUuid>

class QLabel;
class QListWidget;

class InfoDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit InfoDockWidget(QWidget *parent = 0);
    void setConnectionId(const QUuid &uuid);
    void clearText();
public slots:


private:
    void createControls();
    void updateInformation();
    QUuid conId;
    QLabel *infoLabel;
    QListWidget *list;
};

#endif // UDPCLIENTINFODOCKWIDGET_H
