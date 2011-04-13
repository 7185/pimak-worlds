#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "SettingsWindow.h"
#include "AboutWindow.h"
#include "Connection.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showRenderZone();

private slots:
    void on_actQuit_triggered();
    void on_actSettings_triggered();
    void on_actAbout_triggered();
    void updateTimer(int);
    void updateList();
    void clientConnect();
    void clientDisconnect();
    void on_actFirstCam_toggled(bool );
    void on_actThirdCam_toggled(bool );
    void on_message_returnPressed();
    void on_whisper_returnPressed();
    void on_actConnect_triggered();
    void on_actDisconnect_triggered();
    void appendMessage(QString="", quint16=0);
    void on_actWhisper_toggled(bool );

private:
    Ui::MainWindow *ui;
    SettingsWindow *settings;
    AboutWindow *about;
    QTimer *paintTimer;
    Connection *connection;
    QStringList *nickColors;
    QLabel *fpsLbl;
    QLabel *posLbl;
};

#endif // MAINWINDOW_H
