#ifndef CONNECTION_H
#define CONNECTION_H

#include "User.h"

#include <QtNetwork>

class Connection : public QObject
{
    Q_OBJECT
    public:
    Connection();

    public slots:
    void dataRecv();
    void dataSend(quint16, QString = "");
    void dataHandler(quint16, QString);
    void socketError(QAbstractSocket::SocketError);

    void displayData(QString);

    public:
    QTcpSocket* getSocket();
    QString getMessage();
    QMap<quint16, User*>* getUsers();
    void clearUserlist();

    signals:
    void messageChanged();
    void listChanged();

    private:
    QMap<quint16, User*> *users;
    QTcpSocket *socket; // server
    quint16 messageSize;
    QString *message;
};

#endif // CONNECTION_H
