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

    private slots:
        void about();
        void openSettingsWindow();
        void appendMessage(QString mes = "",quint16 = 0);
        void updateList();

        void on_connectAction_triggered();
        void on_disconnectAction_triggered();
        void on_message_returnPressed();
        void on_whisper_returnPressed();
        void on_displayWhisperAction_toggled(bool);

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

        QAction *quitAction;
        QAction *settingsAction;
        QAction *displayWhisperAction;
        QAction *displayTimeAction;
        QAction *aboutAction;
        QAction *connectAction;
        QAction *disconnectAction;

};

#endif // MAINWINDOW_H
