#ifndef USER_H
#define USER_H

#include <QtNetwork>
#include <iostream>

class User : public QObject
{
    Q_OBJECT
    public:
        User(quint16 uid, QString nick);
        ~User();
        QString getNickname();
        quint16 getId();

    private:
        QString *nickname;
        quint16 *id;
};

#endif // USER_H
