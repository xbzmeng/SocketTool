#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "infodockwidget.h"
#include "preferencedialog.h"

namespace Ui {
class MainWindow;
}

class QTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

private slots:
    void newConnection();
    void closeTab(int);
    void updateInformation();
    void about();
    void preference();

private:
    void updateSetting();
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    InfoDockWidget *infoDock;
    PreferenceSettings setting;
};

#endif // MAINWINDOW_H
