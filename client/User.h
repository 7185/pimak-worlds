#ifndef USER_H
#define USER_H

#include <QtNetwork>
#include <Ogre.h>

class User : public QObject
{
    Q_OBJECT
    public:
        User(quint16 uid, QString nick);
        ~User();
        QString getNickname();
        quint16 getId();
        void setPosition(float nx, float ny, float nz, float npitch, float nyaw);

    public:
        Ogre::Entity *avatar;
        Ogre::SceneNode *node;
        quint16 id;
        float x;
        float y;
        float z;
        float pitch;
        float yaw;
    private:
        QString *nickname;
};

#endif // USER_H
