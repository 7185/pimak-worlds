#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
        QWidget(parent)
{
    // Settings window
    settings = new QSettings("pimakworlds.ini", QSettings::IniFormat);

    QGridLayout *settingsBoxLayout = new QGridLayout;
    QTabWidget *tabs = new QTabWidget;

    QPushButton *buttonApply = new QPushButton(tr("Apply"));
    QPushButton *buttonCancel = new QPushButton(tr("Cancel"));
    buttonApply->setIcon(QIcon(":/img/dialog-apply.png"));
    buttonCancel->setIcon(QIcon(":/img/dialog-cancel.png"));

    // Connection tab
    QWidget *tabConnection = new QWidget;
    QVBoxLayout *layConnection = new QVBoxLayout;
    QFormLayout *layServer = new QFormLayout;
    QFormLayout *layIdent = new QFormLayout;
    QGroupBox *gbServer = new QGroupBox(tr("Server"));
    QGroupBox *gbIdent = new QGroupBox(tr("Identity"));

    host = new QLineEdit;
    port = new QSpinBox;
    port->setMaximum(65535);
    nickname = new QLineEdit;

    layServer->addRow(tr("Host :"),host);
    layServer->addRow(tr("Port :"),port);
    layIdent->addRow(tr("Nickname :"),nickname);

    gbServer->setLayout(layServer);
    gbIdent->setLayout(layIdent);

    layConnection->addWidget(gbServer);
    layConnection->addWidget(gbIdent);
    tabConnection->setLayout(layConnection);

    // Display tab
    QWidget *tabDisplay = new QWidget;
    QVBoxLayout *layDisplay = new QVBoxLayout;
    QFormLayout *layVideo = new QFormLayout;
    QFormLayout *layChat = new QFormLayout;
    QGroupBox *gbVideo = new QGroupBox(tr("Video"));
    QGroupBox *gbChat = new QGroupBox(tr("Chat"));

    fps = new QSpinBox;
    fps->setMinimum(5);
    fps->setMaximum(500);
    layVideo->addRow(tr("Frame/sec."),fps);

    displayTime = new QCheckBox;
    displayColors = new QCheckBox;
    layChat->addRow(displayTime,new QLabel(tr("Display time")));
    layChat->addRow(displayColors,new QLabel(tr("Colored nicks")));

    gbVideo->setLayout(layVideo);
    gbChat->setLayout(layChat);

    layDisplay->addWidget(gbVideo);
    layDisplay->addWidget(gbChat);
    tabDisplay->setLayout(layDisplay);

    //

    tabs->addTab(tabConnection,tr("Connection"));
    tabs->addTab(tabDisplay,tr("Display"));

    settingsBoxLayout->addWidget(tabs,0,0,2,4);
    settingsBoxLayout->addWidget(buttonCancel,5,2);
    settingsBoxLayout->addWidget(buttonApply,5,3);

    connect(buttonCancel,SIGNAL(clicked()),this,SLOT(close()));
    connect(buttonApply,SIGNAL(clicked()),this,SLOT(applyEvent()));

    setLayout(settingsBoxLayout);

    readSettings();
}

void SettingsWindow::readSettings()
{
    nickname->setText(settings->value("account/name").toString());
    host->setText(settings->value("server/host").toString());
    port->setValue(settings->value("server/port").toInt());
    fps->setValue(settings->value("video/fps").toInt());
    displayColors->setChecked(settings->value("misc/colorednicks").toBool());
    displayTime->setChecked(settings->value("misc/displaytime").toBool());
}

void SettingsWindow::writeSettings()
{
    settings->setValue("account/name",nickname->text());
    settings->setValue("server/host",host->text());
    settings->setValue("server/port",port->value());
    settings->setValue("video/fps",fps->value());
    settings->setValue("misc/colorednicks",displayColors->isChecked());
    settings->setValue("misc/displaytime",displayTime->isChecked());
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

int SettingsWindow::getFps()
{ return fps->value(); }

bool SettingsWindow::getDisplayTime()
{ return displayTime->isChecked(); }

bool SettingsWindow::getDisplayColors()
{ return displayColors->isChecked(); }
