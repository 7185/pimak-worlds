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

#include "Server.h"
#include "Client.h"

Server::Server(int port) {
  QString *serverState = new QString;

  server = new QTcpServer(this);
  if (!server->listen(QHostAddress::Any, static_cast<quint16>(port))) {
    *serverState = tr("Server didn't start. Reason: ") + server->errorString();
  } else {
    *serverState =
        tr("Server started on port ") + QString::number(server->serverPort());
    connect(server, SIGNAL(newConnection()), this, SLOT(clientConnect()));
  }
  std::cout << serverState->toStdString() << std::endl;

  updateTimer = new QTimer;
  updateTimer->start(UPDATE_TICK);
  connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateClients()));
}

void Server::clientConnect() {
  new Client(server->nextPendingConnection(), this);
}

void Server::updateClients() {
  foreach (Client *client, Client::getClients()) {
    client->sendPositionToAll();
  }
}
