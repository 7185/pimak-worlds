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

#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>
#include <iostream>

#define CONNECTION_TIMEOUT 120000

class Client : public QObject {
  Q_OBJECT

 public:
  explicit Client(QTcpSocket *tcp, QObject *parent = nullptr);
  ~Client();
  static QMap<quint16, Client *> getClients();
  void sendPositionToAll();

 public slots:
  static void sendToAll(const quint16 &, const QString & = "");
  void sendTo(quint16, const quint16 &, const QString & = "");
  void sendDataTo(quint16, const quint16 &);
  void sendPacket(const QByteArray &);
  void sendList();
  QString getNickname();
  quint16 getId();

 private slots:
  void dataHandler(quint16 dataCode, QString data);
  void dataRecv();
  void clientDisconnect();

 private:
  QTcpSocket *clientTcp;
  QString *nickname;
  quint16 messageSize;
  quint16 id;
  QTimer *lifetime;
  bool posChanged;
  bool needUpdate;
  static QMap<quint16, Client *> clients;
  float x, y, z;
  float pitch, yaw;
};

#endif  // CLIENT_H
