#include "Connection.h"
#include "Protocol.h"

Connection::Connection()
{
    users = new QMap<quint16, User*>;

    socket = new QTcpSocket();
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataRecv()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    message = new QString;
    messageSize = 0;
}


void Connection::dataSend(quint16 msgCode, QString msgToSend)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << (quint16) msgCode;
    out << msgToSend;
    out.device()->seek(0);
    out << (quint16) (packet.size()-sizeof(quint16));

    socket->write(packet);

}

void Connection::dataRecv()
{
    forever {
    QDataStream in(socket);
    if (messageSize == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
             return;
        in >> messageSize;
    }

    if (socket->bytesAvailable() < messageSize)
        return; // we wait for the end of the message

    quint16 messageCode;
    in >> messageCode;

    if (messageCode < 0x10)
    {
        QString messageText;
        in >> messageText;

        dataHandler(messageCode,messageText);
    }
    else
    {
        quint16 id;
        float x,y,z,pitch,yaw;
        in >> id;
        in >> x;
        in >> y;
        in >> z;
        in >> pitch;
        in >> yaw;
        User *u = users->value(id);
        u->setPosition(x,y,z,pitch,yaw);
        emit userPosition(u);
    }
    // we reset the messageSize to 0, waiting for next data
    messageSize = 0;
    }
}

void Connection::dataHandler(quint16 dataCode, QString data)
{
    QStringList split;
    QString sender;
    QList<quint16> usersInList;

    switch (dataCode) {
    case SC_MSG_PUBLIC:
        split=data.split(":");
        sender=split[0];
        split.removeAt(0);
        data = sender+": "+split.join(":");
        displayData(data,dataCode);
        break;
    case SC_ER_NICKINUSE:
        data = tr("This nick is already used. Please retry with another one");
        displayData(data,dataCode);
        break;
    case SC_ER_ERRONEOUSNICK:
        data = tr("Erroneous nickname. Please retry with another one");
        displayData(data,dataCode);
        break;
    case SC_MSG_EVENT:
        displayData(data,dataCode);
        break;
    case SC_USER_JOIN:
        dataSend(CS_USER_LIST);
        data = data+tr(" just log in");
        displayData(data,dataCode);
        break;
    case SC_USER_PART:
        dataSend(CS_USER_LIST);
        data = data+tr(" has quit");
        displayData(data,dataCode);
        break;
    case SC_USER_LIST:
        if (!data.isEmpty()) {
            foreach(QString pair, data.split(";")) {
                quint16 newId = pair.split(":").at(0).toUShort();
                QString newNickname = pair.split(":").at(1);

                usersInList.append(newId);

                /* NOTE TEMPORAIRE:
                 - un user peut parfaitement changer d'id lorsqu'on est déconnecté
                 - cette méthode garantit l'unicité des ids, mais pas celle des nicks
                 Il faut donc détruire systématiquement la liste des users lorsqu'on est déconnecté
                 et qu'elle se reconstruise dans le bon ordre sur chaque événement (ce qui ne devrait
                 pas poser de problème a priori, vu la structure actuelle du code)
                 */

                if (!users->contains(newId)) { // Creating user
                    User *newUser = new User(newId, newNickname);
                    emit userCreated(newUser);
                    users->insert(newId, newUser);
                }
            }
        }
        foreach(quint16 userId, users->keys()) { // Finding users to delete
            if (!usersInList.contains(userId)) { // Deleting user
                User *oldUser = users->value(userId);
                emit userDeleted(oldUser);
                delete oldUser;
                users->remove(userId);
            }
        }
        emit listChanged();
        break;
    case SC_MSG_PRIVATE:
        split = data.split(":");
        sender=split[0];
        split.removeAt(0);
        data = sender+": "+split.join(":");
        displayData(data,dataCode);
        break;
    default:
        displayData(tr("Unknown message received"));
    }
}

void Connection::socketError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            displayData(tr("ERROR : host not found"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            displayData(tr("ERROR : connection refused. Is the server launched?"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            displayData(tr("ERROR : connection closed by remote host"));
            break;
        default:
            displayData(tr("ERROR : ") + socket->errorString());
    }
}

void Connection::displayData(QString data, quint16 type)
{
    *message = data;
    messageType = type;
    emit messageChanged();
}

void Connection::clearUserlist()
{
    foreach(quint16 id, users->keys())
    {
        User *oldUser = users->value(id);
        emit userDeleted(oldUser);
        delete oldUser;
        users->remove(id);
    }
}

QTcpSocket* Connection::getSocket()
{
    return socket;
}

QMap<quint16, User*>* Connection::getUsers()
{
    return users;
}

quint16 Connection::getIdByNick(QString nick)
{
    foreach(quint16 id, users->keys())
    {
        if (users->value(id)->getNickname() == nick) return id;
    }
    return 0;
}

QString Connection::getMessage()
{
    return *message;
}

quint16 Connection::getMessageType()
{
    return messageType;
}
