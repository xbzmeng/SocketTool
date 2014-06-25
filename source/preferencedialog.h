#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

namespace Ui {
class PreferenceDialog;
}

struct PreferenceSettings {
    bool displayIp;
    bool displayPort;
    bool editableBox;
    QFont boxFont;
};

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = 0);
    ~PreferenceDialog();
    void setUpSettings(const PreferenceSettings &setting);
    void accept();
    PreferenceSettings settings() const;
private slots:
    void chooseFont();
private:
    Ui::PreferenceDialog *ui;
    PreferenceSettings config;
};



#endif // PREFERENCEDIALOG_H
