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
    void dataSend(quint16, QString="");
    void positionSend(float, float, float, float, float);
    void dataHandler(quint16, QString);
    void socketError(QAbstractSocket::SocketError);
    void displayData(QString,quint16=0);

    public:
    QTcpSocket* getSocket();
    QString getMessage();
    quint16 getMessageType();
    QMap<quint16, User*>* getUsers();
    quint16 getIdByNick(QString);
    void clearUserlist();

    signals:
    void messageChanged();
    void listChanged();
    void userCreated(User*);
    void userDeleted(User*);
    void userPosition(User*);

    private:
    QMap<quint16, User*> *users;
    QTcpSocket *socket; // server
    quint16 messageSize;

    QString *message;
    quint16 messageType;
};

#endif // CONNECTION_H
