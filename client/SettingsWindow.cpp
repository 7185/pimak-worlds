#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
        QWidget(parent)
{
    // Settings window


    QGridLayout *settingsBoxLayout = new QGridLayout;
    QFormLayout *formLayout = new QFormLayout;
    QPushButton *buttonApply = new QPushButton("Valider");
    buttonApply->setIcon(QIcon(":/img/dialog-apply.png"));
    QPushButton *buttonCancel = new QPushButton("Annuler");
    buttonCancel->setIcon(QIcon(":/img/dialog-cancel.png"));

    settings = new QSettings("pimakworlds.ini", QSettings::IniFormat);
    nickname = new QLineEdit;
    host = new QLineEdit;
    port = new QSpinBox;

    formLayout->addRow("Adresse :",host);
    formLayout->addRow("Port :",port);
    formLayout->addRow("Pseudo :",nickname);

    settingsBoxLayout->addLayout(formLayout,0,0,2,2);
    settingsBoxLayout->addWidget(buttonCancel,5,1);
    settingsBoxLayout->addWidget(buttonApply,5,2);

    connect(buttonCancel,SIGNAL(clicked()),this,SLOT(close()));
    // TODO: close & writeSettings
    connect(buttonApply,SIGNAL(clicked()),this,SLOT(applyEvent()));

    setLayout(settingsBoxLayout);

    readSettings();

}

void SettingsWindow::readSettings()
{
    nickname->setText(settings->value("account/name").toString());
    host->setText(settings->value("server/host").toString());
    port->setMaximum(65535);
    port->setValue(settings->value("server/port").toInt());
}

void SettingsWindow::writeSettings()
{
    settings->setValue("account/name",nickname->text());
    settings->setValue("server/host",host->text());
    settings->setValue("server/port",port->value());
}

void SettingsWindow::applyEvent() {
    writeSettings();
    close();
}

void SettingsWindow::closeEvent(QCloseEvent *event) {
    readSettings();
    event->accept();
}

QString SettingsWindow::getHost()
{ return host->text(); }

QString SettingsWindow::getNickname()
{ return nickname->text(); }

int SettingsWindow::getPort()
{ return port->value(); }
