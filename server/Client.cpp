#include "Client.h"
#include "Protocol.h"



QMap<QString,Client*> Client::clients;

Client::Client(QTcpSocket *tcp, QObject *p) : QObject(p),clientTcp(tcp),messageSize(0)
{
    // QTcpSocket must be destroyed with Client
    tcp->setParent(this);

    QString nickname = "undefined";

    connect(clientTcp,SIGNAL(disconnected()), SLOT(clientDisconnect()));
    connect(clientTcp,SIGNAL(readyRead()), SLOT(dataRecv()));
}

Client::~Client()
{
    clients.remove(nickname);
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

        std::cout << "[From: " << nickname.toStdString() << "] " << messageCode << ": " << message.toStdString() << std::endl;
        dataHandler(messageCode, message);

    }
}

void Client::dataHandler(quint16 dataCode, QString data)
{

    QStringList splitted;
    QString receiver;

    switch (dataCode) {
    case CS_AUTH:
        nickname=data;
        // Adding the client to the list
        clients.insert(nickname,this);
        sendToAll(SC_JOIN,nickname);
        break;
    case CS_PUBMSG:
        sendToAll(SC_PUBMSG,nickname+": "+data);
        break;
    case CS_USERLIST:
        emit sendList();
        break;
    case CS_PRIVMSG:
        splitted=data.split(" ");
        receiver=splitted[0];
        splitted[0]=nickname+":";
        sendTo(receiver,SC_PRIVMSG,splitted.join(" "));
        break;
    default:
        std::cout << nickname.toStdString() << tr(" a envoyé une requête inconnue !").toStdString() << std::endl;
    }
}


void Client::clientDisconnect()
{
    emit sendToAll(SC_PART,nickname);
    // We let the Client delete itself
    deleteLater();
}

void Client::sendList()
{
    QStringList userlist;
    foreach (QString string, clients.keys())
    {
        userlist.append(string);
    }
    emit sendTo(nickname,SC_USERLIST,userlist.join(" "));
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
void Client::sendTo(QString user,const quint16 &messageCode, const QString &message)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out << (quint16) 0;
    out << (quint16) messageCode;
    out << message;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    clients[user]->sendPacket(packet);
}


void Client::sendPacket(const QByteArray &packet)
{
    clientTcp->write(packet);
}
