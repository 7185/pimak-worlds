#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "SettingsWindow.h"
#include "OgreWidget.h"
#include "Connection.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();
        void showRenderZone();

    private:
        void initActions();
        void initMenus();
        void initConnect();

    private slots:
        void about();
        void openSettingsWindow();
        void appendMessage(QString="",quint16=0);
        void updateList();
        void updateTimer(int);

        void on_connectAct_triggered();
        void on_disconnectAct_triggered();
        void on_message_returnPressed();
        void on_whisper_returnPressed();
        void on_displayWhisperAct_toggled(bool);
        void on_firstCamAct_toggled(bool);
        void on_thirdCamAct_toggled(bool);

        void clientConnect();
        void clientDisconnect();

    private:
        Connection *connection;

        OgreWidget *renderZone;
        QTextEdit *chatZone;
        QLineEdit *message;
        QLineEdit *whisper;
        QComboBox *whisperSelector;
        SettingsWindow *settings;
        QStringList *nickColors;
        QTimer *paintTimer; // FPS

        QAction *quitAct;
        QAction *settingsAct;
        QAction *displayWhisperAct;
        QAction *displayTimeAct;
        QAction *aboutAct;
        QAction *connectAct;
        QAction *disconnectAct;

        QAction *firstCamAct;
        QAction *thirdCamAct;

};

#endif // MAINWINDOW_H
