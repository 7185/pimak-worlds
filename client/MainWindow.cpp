#include "MainWindow.h"
#include "Protocol.h"


MainWindow::MainWindow()
{
    setWindowIcon(QIcon(":/img/icon.png"));
    setWindowTitle(tr("Pimak Worlds"));
    resize(800,600);

    initActions();
    initMenus();

    // Toolbar
    QToolBar *toolB = addToolBar(tr("Toolbar"));
    toolB->setIconSize(QSize(16,16));
    toolB->addAction(firstCamAct);
    toolB->addAction(thirdCamAct);

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
    nickColors = new QStringList;
    nickColors->append("#666666");
    nickColors->append("#0080FF");
    nickColors->append("#00FF00");
    nickColors->append("#FF8000");
    nickColors->append("#FF0080");
    nickColors->append("#00FFFF");
    nickColors->append("#8000FF");

    paintTimer = new QTimer;
    paintTimer->start(1000/settings->getFps()->value()); //ton oeil en voit que 10 par seconde pd

    connect(settings->getFps(),SIGNAL(valueChanged(int)),this,SLOT(updateTimer(int)));
    connect(paintTimer,SIGNAL(timeout()),renderZone,SLOT(update()));

    connect(connection->getSocket(), SIGNAL(connected()), this, SLOT(clientConnect()));
    connect(connection->getSocket(), SIGNAL(disconnected()), this, SLOT(clientDisconnect()));
    connect(connection,SIGNAL(messageChanged()),this, SLOT(appendMessage()));
    connect(connection,SIGNAL(listChanged()),this, SLOT(updateList()));

    QMetaObject::connectSlotsByName(this); // NOTE: define objects' names to connect before calling this
}

void MainWindow::initActions()
{
    // Actions - don't forget to declare them
    connectAct = new QAction(tr("&Connect"),this);
    connectAct->setObjectName("connectAct");
    connectAct->setIcon(QIcon(":/img/gtk-connect.png"));

    disconnectAct = new QAction(tr("&Disconnect"),this);
    disconnectAct->setObjectName("disconnectAct");
    disconnectAct->setIcon(QIcon(":/img/gtk-disconnect.png"));
    disconnectAct->setEnabled(false);

    quitAct = new QAction(tr("&Quit"),this);
    quitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));
    quitAct->setIcon(QIcon(":/img/application-exit.png"));
    quitAct->setStatusTip(tr("Close the application"));
    connect(quitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

    settingsAct = new QAction(tr("&Settings..."),this);
    settingsAct->setShortcut(QKeySequence("Ctrl+P"));
    settingsAct->setIcon(QIcon(":/img/preferences-desktop.png"));
    settingsAct->setStatusTip(tr("Application settings"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));

    displayWhisperAct = new QAction(tr("&Whisper"),this);
    displayWhisperAct->setObjectName("displayWhisperAct");
    displayWhisperAct->setCheckable(true);
    displayWhisperAct->setChecked(true);

    aboutAct = new QAction(tr("&About Pimak Worlds..."),this);
    aboutAct->setIcon(QIcon(":/img/gtk-about.png"));
    aboutAct->setStatusTip(tr("Information about the application"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    // Camera actions
    firstCamAct = new QAction(tr("First person camera"),this);
    firstCamAct->setIcon(QIcon(":/img/eyes.png"));
    firstCamAct->setCheckable(true);
    firstCamAct->setChecked(true);
    firstCamAct->setObjectName("firstCamAct");

    thirdCamAct = new QAction(tr("Third person camera"),this);
    thirdCamAct->setIcon(QIcon(":/img/camera-video.png"));
    thirdCamAct->setCheckable(true);
    thirdCamAct->setChecked(false);
    thirdCamAct->setObjectName("thirdCamAct");

}

void MainWindow::initMenus()
{
    // Menus
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(connectAct);
    fileMenu->addAction(disconnectAct);
    fileMenu->addAction(quitAct);
    QMenu *displayMenu = menuBar()->addMenu(tr("&View"));
    displayMenu->addAction(displayWhisperAct);
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(settingsAct);
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
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

void MainWindow::on_displayWhisperAct_toggled(bool checked)
{
    whisperSelector->setVisible(checked);
    whisper->setVisible(checked);
}

void MainWindow::on_firstCamAct_toggled(bool checked)
{
    if (checked) renderZone->setActiveCam(false);
    thirdCamAct->setChecked(!checked);
}

void MainWindow::on_thirdCamAct_toggled(bool checked)
{
    if (checked) renderZone->setActiveCam(true);
    firstCamAct->setChecked(!checked);
}

void MainWindow::on_connectAct_triggered()
{
    appendMessage(tr("Connecting to ")+settings->getHost()->text()+":"+QString::number(settings->getPort()->value())+tr("..."));
    connection->getSocket()->abort(); // Closing old connexions
    connection->getSocket()->connectToHost(settings->getHost()->text(), settings->getPort()->value());
}

void MainWindow::on_disconnectAct_triggered()
{
    connection->getSocket()->disconnectFromHost();
}

void MainWindow::on_message_returnPressed()
{
    appendMessage(settings->getNickname()->text()+": "+message->text(),SC_PUBMSG);
    connection->dataSend(CS_PUBMSG, message->text());
    message->clear();
    message->setFocus();
}

void MainWindow::on_whisper_returnPressed()
{
    appendMessage("("+tr("to: ")+whisperSelector->currentText()+") "+whisper->text(),SC_PRIVMSG);
    connection->dataSend(CS_PRIVMSG,QString::number(connection->getIdByNick(whisperSelector->currentText()))+":"+whisper->text());
    whisper->clear();
    whisper->setFocus();
}

void MainWindow::clientConnect()
{
    connection->dataSend(CS_AUTH,settings->getNickname()->text());
    appendMessage(tr("Connection successful"));
    connectAct->setEnabled(false);
    disconnectAct->setEnabled(true);
    message->setEnabled(true);
}

void MainWindow::clientDisconnect()
{
    connection->clearUserlist();
    appendMessage(tr("Disconnected from the server"));
    connectAct->setEnabled(true);
    disconnectAct->setEnabled(false);
    message->setEnabled(false);
    whisper->setEnabled(false);
    whisperSelector->clear();
    whisperSelector->setEnabled(false);
}

void MainWindow::appendMessage(QString mes, quint16 type)
{
    if (mes.isEmpty() && type==0)
    {
        mes = connection->getMessage();
        type = connection->getMessageType();
    }

    QStringList splitted;
    switch (type)
    {
    case SC_PUBMSG:
        if (settings->getDisplayColors()) {
            splitted = mes.split(":");
            if (connection->getIdByNick(splitted[0]) == 0) splitted[0] = "<span style=\"color:"+nickColors->at(0)+";font-weight:bold;\">"+splitted[0]+"</span>";
            else splitted[0] = "<span style=\"color:"+nickColors->at(connection->getIdByNick(splitted[0])%(nickColors->length()-1)+1)+";\">"+splitted[0]+"</span>";
            mes = splitted.join(":");
        }
        mes = "<span style=\"color:black;font-weight:normal;font-style:normal\">"+mes+"</span>";
    case SC_EVENT:
        mes = "<span style=\"color:orange;font-weight:bold;font-style:italic;\">"+mes+"</span>";
    case SC_JOIN:
        mes = "<span style=\"color:green;font-weight:normal;font-style:italic;\">"+mes+"</span>";
    case SC_PART:
        mes = "<span style=\"color:brown;font-weight:normal;font-style:italic;\">"+mes+"</span>";
    case SC_PRIVMSG:
        mes = "<span style=\"color:blue;font-weight:normal;font-style:italic;\">"+mes+"</span>";
    case SC_NICKINUSE:
    case SC_ERRONEOUSNICK:
        mes = "<span style=\"color:red;font-weight:bold;font-style:normal;\">"+mes+"</span>";
    default:
        mes = "<span style=\"color:black;font-weight:bold;font-style:normal;\">"+mes+"</span>";
    }

    if (settings->getDisplayTime()) mes = QDateTime::currentDateTime().toString("[hh:mm:ss] ")+mes;
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

void MainWindow::updateTimer(int i)
{
    paintTimer->stop();
    paintTimer->start(1000/i);
}

void MainWindow::showRenderZone()
{
    renderZone->show();
}
