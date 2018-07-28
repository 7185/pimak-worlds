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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QCloseEvent>
#include <QSettings>
#include <QtWidgets/QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog {
  Q_OBJECT

 public:
  explicit SettingsWindow(QWidget *parent = 0);
  ~SettingsWindow();
  void readSettings();
  void writeSettings();
  QString getHost();
  QString getNickname();
  quint16 getPort();
  int getFps();
  bool getDisplayTime();
  bool getDisplayColors();
 signals:
  void fpsChanged(int);

 protected:
  void closeEvent(QCloseEvent *event);

 private slots:
  void on_buttonBox_accepted();
  void on_buttonBox_rejected();

 private:
  Ui::SettingsWindow *ui;
  QSettings *settings;
};

#endif  // SETTINGSWINDOW_H
