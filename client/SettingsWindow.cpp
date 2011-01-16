#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
        QWidget(parent)
{
    // Fenetre Options

    readSettings();

    QGridLayout *settingsBoxLayout = new QGridLayout;
    QFormLayout *formLayout = new QFormLayout;
    QPushButton *buttonApply = new QPushButton("Valider");
    buttonApply->setIcon(QIcon(":/img/dialog-apply.png"));
    QPushButton *buttonCancel = new QPushButton("Annuler");
    buttonCancel->setIcon(QIcon(":/img/dialog-cancel.png"));

    formLayout->addRow("Adresse :",host);
    formLayout->addRow("Port :",port);
    formLayout->addRow("Pseudo :",nickname);

    settingsBoxLayout->addLayout(formLayout,0,0,2,2);
    settingsBoxLayout->addWidget(buttonCancel,5,1);
    settingsBoxLayout->addWidget(buttonApply,5,2);

    // TODO: close & initSettings
    connect(buttonCancel,SIGNAL(clicked()),this,SLOT(close()));

    // TODO: close & writeSettings
    connect(buttonApply,SIGNAL(clicked()),this,SLOT(close()));


    setLayout(settingsBoxLayout);
}

void SettingsWindow::readSettings()
{
    QSettings *settings = new QSettings("pimakworlds.ini", QSettings::IniFormat);
    // TODO: settings->setValue("account/name",nickname->text()); À régler dans une boîte spéciale et à sync()
    nickname = new QLineEdit;
    nickname->setText(settings->value("account/name").toString());
    host = new QLineEdit;
    host->setText(settings->value("server/host").toString());
    port = new QSpinBox;
    port->setMaximum(65535);
    port->setValue(settings->value("server/port").toInt());

}

QString SettingsWindow::getHost()
{ return host->text(); }

QString SettingsWindow::getNickname()
{ return nickname->text(); }

int SettingsWindow::getPort()
{ return port->value(); }
