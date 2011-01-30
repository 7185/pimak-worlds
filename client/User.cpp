#include "User.h"

User::User(quint16 uid, QString nick)
{
    id = uid;
    nickname = nick;
}

User::~User()
{
}

QString User::getNickname()
{
    return nickname;
}

quint16 User::getId() {
    return id;
}

