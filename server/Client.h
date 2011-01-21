#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>
#include <iostream>

class Client : public QObject
{
    Q_OBJECT

    public:
        explicit Client(QTcpSocket *tcp, QObject *parent = 0);
        ~Client();

    public slots:
        static void sendToAll(const quint16 &, const QString & = "");
        void sendTo(QString, const quint16 &, const QString & = "");
        void sendPacket(const QByteArray &);
        void sendList();

    private slots:
        void dataHandler(quint16 dataCode, QString data);
        void dataRecv();
        void clientDisconnect();

    private:
        QTcpSocket *clientTcp;
        QString nickname;
        quint16 messageSize;
        static QMap<QString, Client*> clients;
};

#endif // CLIENT_H
