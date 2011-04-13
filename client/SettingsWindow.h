#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QCloseEvent>

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();
    void readSettings();
    void writeSettings();

    QString getHost();
    QString getNickname();
    int getPort();
    int getFps();
    bool getDisplayTime();
    bool getDisplayColors();
signals:
    void fpsChanged(int);

private slots:
     void on_buttonBox_accepted();
     void on_buttonBox_rejected();
protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::SettingsWindow *ui;
    QSettings *settings;
};

#endif // SETTINGSWINDOW_H
