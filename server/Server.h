#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include <iostream>
#include "Client.h"

class Server : public QObject
{
    Q_OBJECT

    public:
        explicit Server(int port);

    private slots:
        void clientConnect();

    private:
        QString *serverState;

        QTcpServer *server;
        QList<Client *> clients;
};

#endif
