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

    QString messageText;
    in >> messageText;

    dataHandler(messageCode,messageText);

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
    case SC_PUBMSG:
        split=data.split(":");
        sender=split[0];
        split.removeAt(0);
        data = "<span style=\"color:black;\">"+sender+": "+split.join(":")+"</span>";
        displayData(data);
        emit listChanged();
        break;
    case SC_NICKINUSE:
        data = "<span style=\"color:red;\"><strong>"+tr("This nick is already used. Please retry with another one")+"</strong></span>";
        displayData(data);
        socket->disconnectFromHost(); // FIXME: should be managed by the server
        break;
    case SC_ERRONEOUSNICK:
        data = "<span style=\"color:red;\"><strong>"+tr("Erroneous nickname. Please retry with another one")+"</strong></span>";
        displayData(data);
        socket->disconnectFromHost(); // FIXME: should be managed by the server
        break;
    case SC_EVENT:
        data = "<em>"+data+"</em>";
        displayData(data);
        break;
    case SC_JOIN:
        dataSend(CS_USERLIST);
        data = "<span style=\"color:green;\"><em>"+data+tr(" just log in")+"</em></span>";
        displayData(data);
        break;
    case SC_PART:
        dataSend(CS_USERLIST);
        data = "<span style=\"color:brown;\"><em>"+data+tr(" has quit")+"</em></span>";
        displayData(data);
        break;
    case SC_USERLIST:
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
                    users->insert(newId, new User(newId, newNickname));
                    std::cout << "Creating user " << newId << std::endl;
                }
            }
        }
        foreach(quint16 userId, users->keys()) { // Finding users to delete
            if (!usersInList.contains(userId)) { // Deleting user
               delete users->value(userId);
               users->remove(userId);
               std::cout << "Deleting user " << userId << std::endl;
            }
        }

        emit listChanged();
        break;
    case SC_PRIVMSG:
        split = data.split(":");
        sender=split[0];
        split.removeAt(0);
        data = "<span style=\"color:blue;\"><em>"+sender+": "+split.join(":")+"</em></span>";
        displayData(data);
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
            displayData(tr("<strong>ERROR : host not found</strong>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            displayData(tr("<strong>ERROR : connection refused. Is the server launched?</strong>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            displayData(tr("<strong>ERROR : connection closed by remote host</strong>"));
            break;
        default:
            displayData(tr("<strong>ERROR : ") + socket->errorString() + tr("</strong>"));
    }
}

void Connection::displayData(QString data)
{
    *message = data;
    emit messageChanged();
}

void Connection::clearUserlist()
{
    foreach(quint16 id, users->keys())
    {
        delete users->value(id);
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
}

QString Connection::getMessage()
{
    return *message;
}

