#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newconnectiondialog.h"
#include "connection.h"
#include "connectionmanager.h"
#include "tcpserverform.h"
#include "connectionstoragemanager.h"
#include "preferencedialog.h"
#include "clientform.h"
#include <QLabel>
#include <QSettings>
#include <QMessageBox>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug("loaded main ui");

    ui->actionQuit->setShortcut(QKeySequence::Quit);

    //make connnections
    connect(ui->actionNewConnect, SIGNAL(triggered()), this, SLOT(newConnection()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionPreference, SIGNAL(triggered()), this, SLOT(preference()));

    //central widget
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateInformation()));
    setCentralWidget(tabWidget);

    //doc widget
    infoDock = new InfoDockWidget;
    addDockWidget(Qt::RightDockWidgetArea, infoDock);

    //adjust toolbar
    QLabel *label = new QLabel;
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolbar->addWidget(label);
    ui->mainToolbar->setMovable(true);
    qDebug("toolbar created");

    //load saved connections
    ConnectionStorageManager::manager()->loadConnections();
    qDebug("saved connections loaded");

    statusBar()->showMessage(tr("Loaded"));

    QSettings settings("Jonathan", "Socket Tool");
    setting.displayIp = settings.value("displayIp", true).toBool();
    setting.displayPort = settings.value("displayPort", true).toBool();
    setting.editableBox = settings.value("editableBox", true).toBool();
    setting.boxFont = settings.value("boxFont", qApp->font()).value<QFont>();
    qDebug("application settings loaded");
    updateSetting();
}

void MainWindow::updateSetting()
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        ConnectionForm *w = qobject_cast<ConnectionForm *>(tabWidget->widget(i));
        w->setTextFont(setting.boxFont);
        w->setBoxEditable(setting.editableBox);
        w->setDisplayIP(setting.displayIp);
        w->setDisplayPort(setting.displayPort);
    }
    qDebug("settings applied");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newConnection()
{
    NewConnectionDialog connectDlg(this);
    if (connectDlg.exec() == QDialog::Accepted) {
        ConnectionType connectionType = connectDlg.connectionType();
        ConnectionForm *form = nullptr;
        QString formType;
        switch(connectionType) {
        case CONNECTION_UDP_CLIENT:
            form = new ClientForm(tabWidget);
            formType = tr("UDP Client");
            qDebug("UDP client form created");
        break;
        case CONNECTION_TCP_CLIENT:
            form = new ClientForm(tabWidget, false);
            formType = tr("TCP Client");
            qDebug("TCP client form created");
        break;
        case CONNECTION_UDP_SERVER:
            form = new ClientForm(tabWidget);
            formType = tr("UDP Server");
            qDebug("UDP server form created");
        break;
        case CONNECTION_TCP_SERVER:
            form = new TcpServerForm(tabWidget);
            formType = tr("TCP Server");
            qDebug("TCP server form created");
        break;
        }
        form->setTextFont(setting.boxFont);
        form->setBoxEditable(setting.editableBox);
        form->setDisplayIP(setting.displayIp);
        form->setDisplayPort(setting.displayPort);
        QUuid connectionId = connectDlg.connectionId();
        form->setConnectionId(connectionId);
        tabWidget->addTab(form, formType);
        tabWidget->setCurrentWidget(form);

        qDebug("making connections");
        ConnectionManager::manager()->findConnection(connectionId)->linkUp();
        infoDock->setConnectionId(connectionId);
    }
}

void MainWindow::closeTab(int index)
{
    ConnectionForm *w = qobject_cast<ConnectionForm *>(tabWidget->widget(index));

    qDebug("removing connections");
    ConnectionManager *manager = ConnectionManager::manager();
    manager->removeConnection(w->connectionId());
    tabWidget->removeTab(index);

    qDebug("tab closed");
    delete w;

    updateInformation();
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
    while (tabWidget->count()) {
        closeTab(0);
    }
    qDebug("all the tabs removed");
    qDebug("saving connections");
    ConnectionStorageManager::manager()->saveConnections();

    QSettings settings("Jonathan", "Socket Tool");
    settings.setValue("displayIp", setting.displayIp);
    settings.setValue("displayPort", setting.displayPort);
    settings.setValue("editableBox", setting.editableBox);
    settings.setValue("boxFont", setting.boxFont);
    qDebug("application settings saved");

    delete ConnectionManager::manager();
    qDebug("connection manager deleted.");

    delete ConnectionStorageManager::manager();
    qDebug("connection storage manager deleted.");

    qDebug("close event accepted");
    QMainWindow::closeEvent(evt);
}

void MainWindow::updateInformation()
{
    ConnectionForm *w = qobject_cast<ConnectionForm *>(tabWidget->widget(tabWidget->currentIndex()));
    if (w == NULL) {
        qDebug("no tabs now");
        infoDock->clearText();
        return;
    }
    qDebug("displaying connection %s", qPrintable(w->connectionId().toString()));
    infoDock->setConnectionId(w->connectionId());
}

void MainWindow::about()
{
    QMessageBox msgBox;
    qDebug("displaying about dialog");
    msgBox.setWindowTitle(tr("About Socket Tool"));
    msgBox.setText(tr("<h1>Socket Tool</h1>"
                      "<p>Version: 1.1</p>"
                      "<p>Simple tool for testing socket.</p>"
                      "<p>Author: xbzmeng</p>"
                      "<p>Email: <a href=\"mailto:xbzmeng@gmail.com\">xbzmeng@gmail.com</a></p>"
                      "<p>Copyright 2014 xbzmeng. All rights reserved.</p>"));
    msgBox.setIconPixmap(QPixmap(":/resources/icons/connect.png"));
    msgBox.exec();
}

void MainWindow::preference()
{
    PreferenceDialog dlg(this);
    dlg.setUpSettings(setting);
    qDebug("displaying preference dialog");
    if (dlg.exec() == QDialog::Accepted) {
        setting = dlg.settings();
        updateSetting();
    }
}
