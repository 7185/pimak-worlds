#include "User.h"

User::User(quint16 uid, QString nick)
{
    id = uid;
    nickname = new QString(nick);
    x = 0;
    y = 0;
    z = 0;
    pitch = 0;
    yaw = 0;
}

User::~User()
{
}

QString User::getNickname()
{
    return *nickname;
}

quint16 User::getId() {
    return id;
}

void User::setPosition(float nx, float ny, float nz, float npitch, float nyaw)
{
    x = nx;
    y = ny;
    z = nz;
    pitch = npitch;
    yaw = nyaw;
}
