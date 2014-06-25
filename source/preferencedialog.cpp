#include "preferencedialog.h"
#include "ui_preferencedialog.h"
#include <QFontDialog>

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    connect(ui->chooseFontBtn, SIGNAL(clicked()), this, SLOT(chooseFont()));
    connect(ui->displayIpBox, SIGNAL(toggled(bool)), ui->displayPortBox, SLOT(setEnabled(bool)));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::setUpSettings(const PreferenceSettings &setting)
{
    ui->displayIpBox->setChecked(setting.displayIp);
    ui->displayPortBox->setChecked(setting.displayPort);
    ui->receiveBoxEditableBox->setChecked(setting.editableBox);
    ui->chooseFontBtn->setText(setting.boxFont.family());
    ui->chooseFontBtn->setFont(setting.boxFont);
    ui->displayPortBox->setEnabled(setting.displayIp);
}

void PreferenceDialog::chooseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->chooseFontBtn->font(), this);
    if (ok) {
        ui->chooseFontBtn->setFont(font);
        ui->chooseFontBtn->setText(font.family());
    }
}

void PreferenceDialog::accept()
{
    config.displayIp = ui->displayIpBox->isChecked();
    config.displayPort = ui->displayPortBox->isChecked();
    config.editableBox = ui->receiveBoxEditableBox->isChecked();
    config.boxFont = ui->chooseFontBtn->font();
    QDialog::accept();
}

PreferenceSettings PreferenceDialog::settings() const
{
    return config;
}
