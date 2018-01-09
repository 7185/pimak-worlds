/*
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include "SettingsWindow.h"
#include "AboutWindow.h"
#include "Connection.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
 Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void showRenderZone();

 private slots:
  void on_actQuit_triggered();
  void on_actSettings_triggered();
  void on_actAbout_triggered();
  void updateTimer(int);
  void updateWhisperList();
  void clientConnect();
  void clientDisconnect();
  void on_actFirstCam_toggled(bool );
  void on_actThirdCam_toggled(bool );
  void on_message_returnPressed();
  void on_whisper_returnPressed();
  void on_actConnect_triggered();
  void on_actDisconnect_triggered();
  void appendMessage(QString="", quint16=0);
  void on_actWhisper_toggled(bool );

private:
  Ui::MainWindow *ui;
  SettingsWindow *settings;
  AboutWindow *about;
  QTimer *paintTimer;
  QTimer *posTimer;
  Connection *connection;
  QStringList *nickColors;
  QLabel *fpsLbl;
  QLabel *posLbl;
};

#endif // MAINWINDOW_H
