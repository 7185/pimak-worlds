#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork>
#include <QtGui>
#include "SettingsWindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();

    private:
        void initActions();
        void initMenus();

    private slots:
        void about();
        void openSettingsWindow();
        void appendData(QString);

        void on_connectAction_triggered();
        void on_disconnectAction_triggered();
        void on_message_returnPressed();
        void on_whisper_returnPressed();
        void on_displayWhisperAction_toggled(bool checked);

        void dataRecv();
        void dataSend(quint16,QString);
        void dataHandler(quint16 dataCode, QString data);
        void clientConnect();
        void clientDisconnect();
        void socketError(QAbstractSocket::SocketError erreur);

    private:
        QTcpSocket *socket; // serveur
        quint16 messageSize;
        QWidget *renderZone;
        QTextEdit *chatZone;
        QLineEdit *message;
        QLineEdit *whisper;
        QComboBox *users;
        SettingsWindow *settings;


        QAction *quitAction;
        QAction *settingsAction;
        QAction *displayWhisperAction;
        QAction *displayTimeAction;
        QAction *aboutAction;
        QAction *connectAction;
        QAction *disconnectAction;

};

#endif // MAINWINDOW_H
