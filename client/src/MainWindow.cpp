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

#include "MainWindow.h"
#include "OgreWindow.h"
#include "Protocol.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ogreWindow = new OgreWindow();
  ogreWindow->setFlags(Qt::FramelessWindowHint);
  QWidget* container = QWidget::createWindowContainer(ogreWindow);

  ui->renderZone->setParent(NULL);
  ui->renderZone->deleteLater();
  ui->splitter->insertWidget(0, container);

  settings = new SettingsWindow;
  about = new AboutWindow;
  connection = new Connection;
  connect(connection->getSocket(), SIGNAL(connected()), this,
          SLOT(clientConnect()));
  connect(connection->getSocket(), SIGNAL(disconnected()), this,
          SLOT(clientDisconnect()));
  ;
  ;
  connect(connection, SIGNAL(messageChanged()), this, SLOT(appendMessage()));
  connect(connection, SIGNAL(listChanged()), this, SLOT(updateWhisperList()));
  connect(connection, SIGNAL(userCreated(User*)), ogreWindow,
          SLOT(createAvatar(User*)));
  connect(connection, SIGNAL(userDeleted(User*)), ogreWindow,
          SLOT(destroyAvatar(User*)));
  connect(connection, SIGNAL(userPosition(User*)), ogreWindow,
          SLOT(moveAvatar(User*)));

  // paintTimer = new QTimer;
  // paintTimer->start(1000/settings->getFps()); // ton oeil en voit que 10 par
  // seconde pd
  // connect(paintTimer,SIGNAL(timeout()),ogreWindow,SLOT(requestUpdate()));
  // connect(settings,SIGNAL(fpsChanged(int)),this,SLOT(updateTimer(int)));

  posTimer = new QTimer;
  posTimer->start(1000 / 5);
  connect(posTimer, SIGNAL(timeout()), ogreWindow, SLOT(posSend()));
  connect(ogreWindow, SIGNAL(positionSend(float, float, float, float, float)),
          connection, SLOT(positionSend(float, float, float, float, float)));

  nickColors = new QStringList;
  nickColors->append("#666666");
  nickColors->append("#0080FF");
  nickColors->append("#00FF00");
  nickColors->append("#FF8000");
  nickColors->append("#FF0080");
  nickColors->append("#00FFFF");
  nickColors->append("#8000FF");

  fpsLbl = new QLabel;
  fpsLbl->setFrameStyle(QFrame::Box | QFrame::Sunken);
  posLbl = new QLabel;
  posLbl->setFrameStyle(QFrame::Box | QFrame::Sunken);
  ui->statusBar->addPermanentWidget(posLbl);
  ui->statusBar->addPermanentWidget(fpsLbl);
  ui->mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

  connect(ogreWindow, SIGNAL(dispAverageFps(QString)), fpsLbl,
          SLOT(setText(QString)));
  connect(ogreWindow, SIGNAL(dispPosition(QString)), posLbl,
          SLOT(setText(QString)));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showRenderZone() { ogreWindow->show(); }

void MainWindow::updateTimer(int i) {
  paintTimer->stop();
  paintTimer->start(1000 / i);
}

void MainWindow::on_actQuit_triggered() { qApp->quit(); }

void MainWindow::on_actSettings_triggered() { settings->show(); }

void MainWindow::on_actAbout_triggered() { about->show(); }

void MainWindow::on_actFirstCam_toggled(bool checked) {
  if (checked) ogreWindow->setActiveCam(false);
  ui->actThirdCam->setChecked(!checked);
}

void MainWindow::on_actThirdCam_toggled(bool checked) {
  if (checked) ogreWindow->setActiveCam(true);
  ui->actFirstCam->setChecked(!checked);
}

void MainWindow::on_message_returnPressed() {
  appendMessage(settings->getNickname() + ": " + ui->message->text(),
                SC_MSG_PUBLIC);
  connection->dataSend(CS_MSG_PUBLIC, ui->message->text());
  ui->message->clear();
  ui->message->setFocus();
}

void MainWindow::on_whisper_returnPressed() {
  appendMessage("(" + tr("to: ") + ui->whisperSelector->currentText() + ") " +
                    ui->whisper->text(),
                SC_MSG_PRIVATE);
  connection->dataSend(CS_MSG_PRIVATE,
                       QString::number(connection->getIdByNick(
                           ui->whisperSelector->currentText())) +
                           ":" + ui->whisper->text());
  ui->whisper->clear();
  ui->whisper->setFocus();
  ogreWindow->createAvatar(connection->getUsers()->value(
      connection->getIdByNick(ui->whisperSelector->currentText())));
}

void MainWindow::on_actConnect_triggered() {
  appendMessage(tr("Connecting to ") + settings->getHost() + ":" +
                QString::number(settings->getPort()) + tr("..."));
  connection->getSocket()->abort();  // Closing old connexions
  connection->getSocket()->connectToHost(settings->getHost(),
                                         settings->getPort());
}

void MainWindow::on_actDisconnect_triggered() {
  connection->getSocket()->disconnectFromHost();
}

void MainWindow::clientConnect() {
  connection->dataSend(CS_AUTH, settings->getNickname());
  appendMessage(tr("Connection successful"));
  ui->actConnect->setEnabled(false);
  ui->actDisconnect->setEnabled(true);
  ui->message->setEnabled(true);
}

void MainWindow::clientDisconnect() {
  connection->clearUserlist();
  appendMessage(tr("Disconnected from the server"));
  ui->actConnect->setEnabled(true);
  ui->actDisconnect->setEnabled(false);
  ui->message->setEnabled(false);
  ui->whisper->setEnabled(false);
  ui->whisperSelector->clear();
  ui->whisperSelector->setEnabled(false);
}

void MainWindow::updateWhisperList() {
  ui->whisperSelector->clear();
  if (connection->getUsers()->keys().isEmpty()) {
    ui->whisper->setEnabled(false);
    ui->whisperSelector->setEnabled(false);
  } else {
    foreach (quint16 userid, connection->getUsers()->keys()) {
      ui->whisperSelector->addItem(
          connection->getUsers()->value(userid)->getNickname());
    }
    ui->whisper->setEnabled(true);
    ui->whisperSelector->setEnabled(true);
  }
}

void MainWindow::appendMessage(QString mes, quint16 type) {
  if (mes.isEmpty() && type == 0) {
    mes = connection->getMessage();
    type = connection->getMessageType();
  }

  mes = mes.toHtmlEscaped();

  QStringList splitted;
  switch (type) {
    case SC_MSG_PUBLIC:
      if (settings->getDisplayColors()) {
        splitted = mes.split(":");
        if (connection->getIdByNick(splitted[0]) == 0)
          splitted[0] = "<span style=\"color:" + nickColors->at(0) +
                        ";font-weight:bold;\">" + splitted[0] + "</span>";
        else
          splitted[0] = "<span style=\"color:" +
                        nickColors->at(connection->getIdByNick(splitted[0]) %
                                           (nickColors->length() - 1) +
                                       1) +
                        ";\">" + splitted[0] + "</span>";
        mes = splitted.join(":");
      }
      mes =
          "<span style=\"color:black;font-weight:normal;font-style:normal\">" +
          mes + "</span>";
      break;
    case SC_MSG_EVENT:
      mes =
          "<span style=\"color:orange;font-weight:bold;font-style:italic;\">" +
          mes + "</span>";
      break;
    case SC_USER_JOIN:
      mes =
          "<span style=\"color:green;font-weight:normal;font-style:italic;\">" +
          mes + "</span>";
      break;
    case SC_USER_PART:
      mes =
          "<span style=\"color:brown;font-weight:normal;font-style:italic;\">" +
          mes + "</span>";
      break;
    case SC_MSG_PRIVATE:
      mes =
          "<span style=\"color:blue;font-weight:normal;font-style:italic;\">" +
          mes + "</span>";
      break;
    case SC_ER_NICKINUSE:
      break;
    case SC_ER_ERRONEOUSNICK:
      mes = "<span style=\"color:red;font-weight:bold;font-style:normal;\">" +
            mes + "</span>";
      break;
    default:
      mes = "<span style=\"color:black;font-weight:bold;font-style:normal;\">" +
            mes + "</span>";
      break;
  }

  if (settings->getDisplayTime())
    mes = QDateTime::currentDateTime().toString("[hh:mm:ss] ") + mes;
  ui->chatZone->append(mes);
}

void MainWindow::on_actWhisper_toggled(bool checked) {
  ui->whisperSelector->setVisible(checked);
  ui->whisper->setVisible(checked);
}
