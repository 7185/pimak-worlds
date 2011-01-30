#include "Server.h"
#include "Client.h"


Server::Server(int port)
{
    QString *serverState = new QString;

    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, port))
    {
        *serverState = tr("Server didn't start. Reason: ") + server->errorString();
    }
    else
    {
        *serverState = tr("Server started on port ") + QString::number(server->serverPort());
        connect(server, SIGNAL(newConnection()), this, SLOT(clientConnect()));
    }
    std::cout << serverState->toStdString() << std::endl;
}

void Server::clientConnect()
{
    Client *newClient = new Client(server->nextPendingConnection(), this);
}


