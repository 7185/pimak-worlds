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

    public:
        Ogre::Entity *avatar;
        Ogre::SceneNode *node;
    private:
        QString *nickname;
        quint16 id;
};

#endif // USER_H
