#include "Client.h"
#include "Protocol.h"



QMap<quint16,Client*> Client::clients;

Client::Client(QTcpSocket *tcp, QObject *p) : QObject(p),clientTcp(tcp),messageSize(0)
{
    tcp->setParent(this); // QTcpSocket must be destroyed with Client

    connect(clientTcp,SIGNAL(disconnected()), SLOT(clientDisconnect()));
    connect(clientTcp,SIGNAL(readyRead()), SLOT(dataRecv()));
    
    nickname = new QString;

    for (quint16 i=1;i<=clients.size()+1;i++) {
        if (!clients.contains(i)) id=i;
    }

    clients.insert(id,this); // Adding the client to the list
}

Client::~Client()
{
    clients.remove(id);
}

void Client::dataRecv()
{
    forever
    {
        QDataStream in(clientTcp);

        if (messageSize == 0)
        {
            if (clientTcp->bytesAvailable() < (int)sizeof(quint16))
                return;
            in >> messageSize;
        }
        if (clientTcp->bytesAvailable() < messageSize)
            return;
        messageSize = 0; // MARK IT ZERO!

        quint16 messageCode;
        in >> messageCode;

        QString message;
        in >> message;

        if (nickname->isEmpty())
            std::cout << "[From: " << id << "] (" << messageCode << ") " << message.toStdString() << std::endl;
        else
            std::cout << "[From: " << id << " (" << nickname->toStdString() << ")] (" << messageCode << ") " << message.toStdString() << std::endl;
        dataHandler(messageCode, message);

    }
}

void Client::dataHandler(quint16 dataCode, QString data)
{
    QStringList splitted;
    QString receiver;
    bool nickInUse = false;

    switch (dataCode) {
    case CS_AUTH:
        foreach (Client *client, clients)
        {
            if (client->getNickname() == data)
            {
                nickInUse = true;
                break;
            }
        }
        if (nickInUse) {
            sendTo(id,SC_NICKINUSE);
            emit clientDisconnect();
            break;
        }
        else if (data.contains(":"))
        {
            sendTo(id,SC_ERRONEOUSNICK);
            emit clientDisconnect();
            break;
        }
        else
        {
            *nickname=data;
            sendToAll(SC_JOIN,*nickname);
            break;
        }
    case CS_PUBMSG:
        foreach (quint16 userId, clients.keys())
        {
            if (userId != id) emit sendTo(userId,SC_PUBMSG,*nickname+":"+data);
        }
        break;
    case CS_USERLIST:
        emit sendList();
        break;
    case CS_PRIVMSG:
        splitted=data.split(":");
        receiver=splitted[0];
        foreach (Client *client, clients)
        {
            if (client->getId() == receiver.toUShort())
            {
                splitted[0] = *nickname; // set the sender name
                sendTo(client->getId(),SC_PRIVMSG,splitted.join(":"));
            }
        }
        break;
    default:
        if (nickname->isEmpty())
            std::cout << "UID " << id << " sent an unknown request!" << std::endl;
        else
            std::cout << "UID " << id << " (" << nickname->toStdString() << ") sent an unknown request!" << std::endl;
            
    }
}


void Client::clientDisconnect()
{
    if (!nickname->isEmpty()) emit sendToAll(SC_PART,*nickname);
    // We let the Client delete itself
    deleteLater();
}

void Client::sendList()
{
    QStringList userlist;
    foreach (Client *client, clients)
    {
        if (client->getId() != id) userlist.append(QString::number(client->getId())+":"+client->getNickname());
    }
    std::cout << "[To: " << id << "] list " << userlist.join(";").toStdString() << std::endl;
    emit sendTo(id,SC_USERLIST,userlist.join(";"));
}

void Client::sendToAll(const quint16 &messageCode, const QString &message)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16) 0;
    out << (quint16) messageCode;
    out << message;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    foreach (Client *client, clients)
    {
        client->sendPacket(packet);
    }
}

void Client::sendTo(quint16 uid,const quint16 &messageCode, const QString &message)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16) 0;
    out << (quint16) messageCode;
    out << message;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    clients[uid]->sendPacket(packet);
    std::cout << "[To: " << uid << "] (" << messageCode << ") " << message.toStdString() << std::endl;
}


void Client::sendPacket(const QByteArray &packet)
{
    clientTcp->write(packet);
    clientTcp->waitForBytesWritten(500);
}

QString Client::getNickname()
{
    return *nickname;
}

quint16 Client::getId() {
    return id;
}

