#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T22:55:18
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): {
    QT += widgets
    CONFIG += c++11
}

CONFIG += embed_manifest_exe

lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c+=11

TARGET = SocketTool
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    newconnectiondialog.cpp \
    connectionmanager.cpp \
    ipctrl.cpp \
    infodockwidget.cpp \
    tcpserverform.cpp \
    connection.cpp \
    connectioninfo.cpp \
    connectionstoragemanager.cpp \
    connectiondisplaydelegate.cpp \
    connectiondisplaymodel.cpp \
    preferencedialog.cpp \
    connectionform.cpp \
    clientform.cpp
HEADERS  += mainwindow.h \
    newconnectiondialog.h \
    connectionmanager.h \
    ipctrl.h \
    infodockwidget.h \
    tcpserverform.h \
    connection.h \
    connectioninfo.h \
    connectionstoragemanager.h \
    connectiondisplaydelegate.h \
    connectiondisplaymodel.h \
    preferencedialog.h \
    connectionform.h \
    clientform.h
FORMS    += mainwindow.ui \
    newconnectiondialog.ui \
    preferencedialog.ui

RESOURCES += \
    resources.qrc

lessThan(QT_MAJOR_VERSION, 5): win32:RC_FILE = socketool.rc
greaterThan(QT_MAJOR_VERSION, 4): win32:RC_ICONS = icon.ico

TRANSLATIONS += zh_CN.ts
