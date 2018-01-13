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

#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow) {
  ui->setupUi(this);

  settings = new QSettings("pimakworlds.ini", QSettings::IniFormat);
  readSettings();
  connect(ui->framesSecSpinBox,SIGNAL(valueChanged(int)),this,SIGNAL(fpsChanged(int)));
}

SettingsWindow::~SettingsWindow() {
  delete ui;
}

void SettingsWindow::readSettings() {
  ui->nicknameLineEdit->setText(settings->value("account/name").toString());
  ui->adressLineEdit->setText(settings->value("server/host").toString());
  ui->portSpinBox->setValue(settings->value("server/port").toInt());
  ui->framesSecSpinBox->setValue(settings->value("video/fps").toInt());
  ui->coloredNicksCheckBox->setChecked(settings->value("misc/colorednicks").toBool());
  ui->displayTimeCheckBox->setChecked(settings->value("misc/displaytime").toBool());
}

void SettingsWindow::writeSettings() {
  settings->setValue("account/name",ui->nicknameLineEdit->text());
  settings->setValue("server/host",ui->adressLineEdit->text());
  settings->setValue("server/port",ui->portSpinBox->value());
  settings->setValue("video/fps",ui->framesSecSpinBox->value());
  settings->setValue("misc/colorednicks",ui->coloredNicksCheckBox->isChecked());
  settings->setValue("misc/displaytime",ui->displayTimeCheckBox->isChecked());
}

QString SettingsWindow::getHost() { return ui->adressLineEdit->text(); }
QString SettingsWindow::getNickname() { return ui->nicknameLineEdit->text(); }
int SettingsWindow::getPort() { return ui->portSpinBox->value(); }
int SettingsWindow::getFps() { return ui->framesSecSpinBox->value(); }
bool SettingsWindow::getDisplayTime() { return ui->displayTimeCheckBox->isChecked(); }
bool SettingsWindow::getDisplayColors() { return ui->coloredNicksCheckBox->isChecked(); }

void SettingsWindow::closeEvent(QCloseEvent *event) {
  readSettings();
  event->accept();
}

void SettingsWindow::on_buttonBox_accepted() {
  writeSettings();
  close();
}

void SettingsWindow::on_buttonBox_rejected() {
  close();
}
