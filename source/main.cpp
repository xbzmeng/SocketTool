#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QtCore>
#include <fstream>

using namespace std;

static ofstream out;

static void loadTranslations()
{
    bool success;
    QTranslator *appTranslator = new QTranslator();
    QString systemLanguage = QLocale::system().name();
    qDebug("detected system language: %s", qPrintable(systemLanguage));
    success = appTranslator->load(systemLanguage);
    if (success) {
        qDebug("load %s translation successfully", qPrintable(systemLanguage));
        qApp->installTranslator(appTranslator);
    } else {
        qWarning("failed to load %s translation", qPrintable(systemLanguage));
    }

    QTranslator *qtTranslator = new QTranslator();
#ifndef Q_OS_WIN32
    success = qtTranslator->load("qt_" + systemLanguage), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#else
    success = qtTranslator->load("qt_" + systemLanguage);
#endif
    if (success) {
        qDebug("load %s qt translation successfully", qPrintable(systemLanguage));
        qApp->installTranslator(qtTranslator);
    } else {
        qWarning("failed to load %s qt translation", qPrintable(systemLanguage));
    }
}

#if QT_VERSION >= 0x050000
void log(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    const char *localMsg = qPrintable(msg);
#else
void log(QtMsgType type, const char* msg)
{
    const char *localMsg = msg;
#endif

    return;

    switch(type) {
    case QtDebugMsg:
        out << "DEBUG::" << localMsg << endl;
        break;
    case QtWarningMsg:
        out << "WARN ::" << localMsg << endl;
        break;
    case QtCriticalMsg:
        out << "CRIT ::" << localMsg << endl;
        break;
    case QtFatalMsg:
        out << "FATAL::" << localMsg << endl;
        abort();
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050000
    qInstallMessageHandler(log);
#else
    qInstallMsgHandler(log);
#endif

    QApplication app(argc, argv);

#ifdef QT_NO_DEBUG
    //log file
    QString logFile = QString("%1/socketool.log").arg(qApp->applicationDirPath());
    out.open(qPrintable(logFile));
#endif

    loadTranslations();

    MainWindow w;
    w.show();

    int result = app.exec();
    out.close();
    return result;
}
