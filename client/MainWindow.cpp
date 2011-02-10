#include "MainWindow.h"
#include "Protocol.h"


MainWindow::MainWindow()
{
    setWindowIcon(QIcon(":/img/icon.png"));
    setWindowTitle("Pimak Worlds");

    initActions();
    initMenus();

    // Statusbar
    QStatusBar *statusB = statusBar();
    statusB->show();

    // Settings window
    settings = new SettingsWindow;

    // Main window
    QWidget *mainHolder = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QSplitter *renderSplitter = new QSplitter;
    renderSplitter->setOrientation(Qt::Vertical);

    renderZone = new OgreWidget(renderSplitter);
    renderZone->hide();

    chatZone = new QTextEdit;
    chatZone->setReadOnly(true);
    
    message = new QLineEdit;
    message->setObjectName("message");
    message->setEnabled(false);

    QHBoxLayout *whisperLayout = new QHBoxLayout;
    whisperSelector = new QComboBox;
    whisper = new QLineEdit;
    whisper->setObjectName("whisper");
    whisper->setEnabled(false);
    whisperSelector->setEnabled(false);
    whisperLayout->addWidget(whisperSelector);
    whisperLayout->addWidget(whisper);

    renderSplitter->addWidget(renderZone);
    renderSplitter->addWidget(chatZone);
    renderSplitter->setCollapsible(0,false);

    mainLayout->addWidget(renderSplitter);
    mainLayout->addWidget(message);
    mainLayout->addLayout(whisperLayout);

    mainHolder->setLayout(mainLayout);
    setCentralWidget(mainHolder);

    connection = new Connection;

    connect(connection->getSocket(), SIGNAL(connected()), this, SLOT(clientConnect()));
    connect(connection->getSocket(), SIGNAL(disconnected()), this, SLOT(clientDisconnect()));
    connect(connection,SIGNAL(messageChanged()),this, SLOT(appendMessage()));
    connect(connection,SIGNAL(listChanged()),this, SLOT(updateList()));

    QMetaObject::connectSlotsByName(this); // NOTE: define objects' names to connect before calling this
}

void MainWindow::initActions()
{
    // Actions - don't forget to declare them
    connectAction = new QAction(tr("Connect"),this);
    connectAction->setObjectName("connectAction");
    connectAction->setIcon(QIcon(":/img/gtk-connect.png"));

    disconnectAction = new QAction(tr("Disconnect"),this);
    disconnectAction->setObjectName("disconnectAction");
    disconnectAction->setIcon(QIcon(":/img/gtk-disconnect.png"));
    disconnectAction->setEnabled(false);

    quitAction = new QAction(tr("Quit"),this);
    quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    quitAction->setIcon(QIcon(":/img/application-exit.png"));
    quitAction->setStatusTip(tr("Close the application"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    settingsAction = new QAction(tr("Settings..."),this);
    settingsAction->setShortcut(QKeySequence("Ctrl+P"));
    settingsAction->setIcon(QIcon(":/img/preferences-desktop.png"));
    settingsAction->setStatusTip(tr("Application settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));

    displayWhisperAction = new QAction(tr("Whisper"),this);
    displayWhisperAction->setObjectName("displayWhisperAction");
    displayWhisperAction->setCheckable(true);
    displayWhisperAction->setChecked(true);

    displayTimeAction = new QAction(tr("Time"),this);
    displayTimeAction->setObjectName("displayTimeAction");
    displayTimeAction->setCheckable(true);
    displayTimeAction->setChecked(false);

    aboutAction = new QAction(tr("About Pimak Worlds..."),this);
    aboutAction->setIcon(QIcon(":/img/gtk-about.png"));
    aboutAction->setStatusTip(tr("Information about the application"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

}

void MainWindow::initMenus()
{
    // Menus
    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(connectAction);
    fileMenu->addAction(disconnectAction);
    fileMenu->addAction(quitAction);
    QMenu *displayMenu = menuBar()->addMenu(tr("View"));
    displayMenu->addAction(displayWhisperAction);
    displayMenu->addAction(displayTimeAction);
    QMenu *toolsMenu = menuBar()->addMenu(tr("Tools"));
    toolsMenu->addAction(settingsAction);
    QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutAction);
}

void MainWindow::openSettingsWindow()
{   settings->show();  }

void MainWindow::about()
{
    // About Dialog
    QDialog *aboutBox = new QDialog(this);
    QGridLayout *aboutBoxLayout = new QGridLayout;
    QLabel *logo = new QLabel(aboutBox);
    QLabel *texte = new QLabel(tr("<h1>Pimak Worlds 0.0.1a</h1><h3>Version numbers are useless</h3><p>Client based on Qt 4.7.0</p>"));
    QPushButton *buttonClose = new QPushButton(tr("Close"));
    logo->setPixmap(QPixmap(":/img/pimak.png"));
    texte->setAlignment(Qt::AlignHCenter);
    logo->setAlignment(Qt::AlignHCenter);
    buttonClose->setIcon(QIcon(":/img/dialog-close.png"));

    connect(buttonClose,SIGNAL(clicked()),aboutBox,SLOT(close()));
    aboutBoxLayout->addWidget(logo,0,0,1,3);
    aboutBoxLayout->addWidget(texte,4,0,1,3);
    aboutBoxLayout->addWidget(buttonClose,5,2);

    aboutBox->setLayout(aboutBoxLayout);
    aboutBox->show();

}

void MainWindow::on_displayWhisperAction_toggled(bool checked)
{
    if (checked) {
        whisperSelector->show();
        whisper->show();
    }
    else {
        whisperSelector->hide();
        whisper->hide();
    }
}

void MainWindow::on_connectAction_triggered()
{
    appendMessage(tr("<strong>Connecting to ")+settings->getHost()+":"+QString::number(settings->getPort())+tr("...</strong>"));
    connection->getSocket()->abort(); // Closing old connexions
    connection->getSocket()->connectToHost(settings->getHost(), settings->getPort());
}

void MainWindow::on_disconnectAction_triggered()
{
    connection->getSocket()->disconnectFromHost();
}

void MainWindow::on_message_returnPressed()
{
    appendMessage("<span style=\"color:black;\">"+settings->getNickname()+": "+message->text()+"</span>");
    connection->dataSend(CS_PUBMSG, message->text());
    message->clear();
    message->setFocus();
}

void MainWindow::on_whisper_returnPressed()
{
    appendMessage("<span style=\"color:blue;\"><em>("+tr("to: ")+whisperSelector->currentText()+") "+whisper->text()+"</em></span>");
    connection->dataSend(CS_PRIVMSG, whisperSelector->currentText()+":"+whisper->text());
    whisper->clear();
    whisper->setFocus();
}

void MainWindow::clientConnect()
{
    connection->dataSend(CS_AUTH,settings->getNickname());
    appendMessage(tr("<strong>Connection successful</strong>"));
    connectAction->setEnabled(false);
    disconnectAction->setEnabled(true);
    message->setEnabled(true);
}

void MainWindow::clientDisconnect()
{
    connection->clearUserlist();
    appendMessage(tr("<strong>Disconnected from the server</strong>"));
    connectAction->setEnabled(true);
    disconnectAction->setEnabled(false);
    message->setEnabled(false);
    whisper->setEnabled(false);
    whisperSelector->clear();
    whisperSelector->setEnabled(false);
}

void MainWindow::appendMessage(QString mes)
{
    if (mes.isEmpty()) mes = connection->getMessage();
    if (displayTimeAction->isChecked()) mes = QDateTime::currentDateTime().toString("[hh:mm:ss] ")+mes;
    chatZone->append(mes);
}

void MainWindow::updateList()
{
    whisperSelector->clear();
    if(connection->getUsers()->keys().isEmpty())
    {
        whisper->setEnabled(false);
        whisperSelector->setEnabled(false);
    }
    else
    {
        foreach(quint16 userid, connection->getUsers()->keys())
        {
            whisperSelector->addItem(connection->getUsers()->value(userid)->getNickname());
        }
        whisper->setEnabled(true);
        whisperSelector->setEnabled(true);
    }
}

void MainWindow::showRenderZone()
{
    renderZone->show();
}
