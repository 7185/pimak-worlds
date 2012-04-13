/**
 * Copyright (c) 2012, Thibault Signor <tibsou@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include "User.h"

#include <QtNetwork>

class Connection : public QObject
{
    Q_OBJECT
    public:
    Connection();

    public slots:
    void dataRecv();
    void dataSend(quint16, QString="");
    void positionSend(float, float, float, float, float);
    void dataHandler(quint16, QString);
    void socketError(QAbstractSocket::SocketError);
    void displayData(QString,quint16=0);

    public:
    QTcpSocket* getSocket();
    QString getMessage();
    quint16 getMessageType();
    QMap<quint16, User*>* getUsers();
    quint16 getIdByNick(QString);
    void clearUserlist();

    signals:
    void messageChanged();
    void listChanged();
    void userCreated(User*);
    void userDeleted(User*);
    void userPosition(User*);

    private:
    QMap<quint16, User*> *users;
    QTcpSocket *socket; // server
    quint16 messageSize;

    QString *message;
    quint16 messageType;
};

#endif // CONNECTION_H
