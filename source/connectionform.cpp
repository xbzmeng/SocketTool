#include "connectionform.h"
#include <QTextEdit>
#include <QToolBar>
#include <QAction>
#include <QComboBox>
#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>
#include "connectionmanager.h"

ConnectionForm::ConnectionForm(QWidget *parent) :
    QWidget(parent)
{
    displayIp = displayPort = true;
    packetMode = true;
    createControls();
}

void ConnectionForm::setTextFont(const QFont &font)
{
    receiveBox->setFont(font);
}

void ConnectionForm::setBoxEditable(bool editable)
{
    receiveBox->setReadOnly(!editable);
}

void ConnectionForm::setDisplayIP(bool display)
{
    displayIp = display;
}

void ConnectionForm::setDisplayPort(bool display)
{
    displayPort = display;
}

void ConnectionForm::createControls()
{
    receiveBox = new QTextEdit;

    utilityBar = new QToolBar;

    QAction *clearAction = new QAction(tr("Clear Text"), this);
    clearAction->setIcon(QIcon(":/resources/icons/edit_clear.png"));
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearText()));

    QAction *exportAction = new QAction(tr("Export..."), this);
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportResult()));
    exportAction->setIcon(QIcon(":/resources/icons/export.png"));

    utilityBar->addAction(exportAction);
    utilityBar->addAction(clearAction);

    QComboBox *combo = new QComboBox;
    combo->addItems(QStringList() << tr("Packet Mode") << tr("Original Data"));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDisplayMode(int)));
    combo->setCurrentIndex(0);

    utilityBar->addWidget(combo);

    utilityBar->setIconSize(QSize(24, 24));
    utilityBar->setStyleSheet("QToolBar{border:0}");
}

void ConnectionForm::dataSended(const QString &data)
{
    QTextCursor textCursor = receiveBox->textCursor();
    textCursor.movePosition(QTextCursor::End);
    receiveBox->setTextCursor(textCursor);
    receiveBox->ensureCursorVisible();

    receiveBox->append(tr("<font color=\"green\">Sended:</font> %1").arg(data));
}

void ConnectionForm::dataReceived(const QString &data)
{
    QTextCursor textCursor = receiveBox->textCursor();
    textCursor.movePosition(QTextCursor::End);
    receiveBox->setTextCursor(textCursor);
    receiveBox->ensureCursorVisible();

    if (packetMode) {
        receiveBox->append(tr("<font color=\"blue\">Received:</font> %1").arg(data));
    } else {
        receiveBox->insertPlainText(tr("%1").arg(data));
    }
}

QUuid ConnectionForm::connectionId() const
{
    return conId;
}

void ConnectionForm::clearText()
{
    receiveBox->clear();
}

void ConnectionForm::exportResult()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save"), "", tr("Text file(*.txt);;All files(*.*)"));
    qDebug("saving result");
    if (!filename.isEmpty()) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical("failed to open file %s", qPrintable(filename));
            QMessageBox::critical(this, tr("File Error"), tr("Failed to save file as %1.").arg(filename));
            return;
        }
        QTextStream out(&file);
        out << receiveBox->toPlainText();
        file.close();
        qDebug("file saved");
    } else {
        qDebug("empty file name");
    }
}

void ConnectionForm::setPacketMode(bool packet)
{
    packetMode = packet;
}

void ConnectionForm::changeDisplayMode(int mode)
{
    packetMode = !mode;
}
