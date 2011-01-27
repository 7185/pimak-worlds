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
    users = new QComboBox;
    whisper = new QLineEdit;
    whisper->setObjectName("whisper");
    whisper->setEnabled(false);
    users->setEnabled(false);
    whisperLayout->addWidget(users);
    whisperLayout->addWidget(whisper);

    renderSplitter->addWidget(renderZone);
    renderSplitter->addWidget(chatZone);

    mainLayout->addWidget(renderSplitter);
    mainLayout->addWidget(message);
    mainLayout->addLayout(whisperLayout);

    mainHolder->setLayout(mainLayout);
    setCentralWidget(mainHolder);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataRecv()));
    connect(socket, SIGNAL(connected()), this, SLOT(clientConnect()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnect()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    messageSize = 0;

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

/* lolcopter
   void MainWindow::on_chatZone_textChanged()
{
    chatZone->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}
*/

void MainWindow::on_displayWhisperAction_toggled(bool checked)
{
    if (checked) {
        users->show();
        whisper->show();
    }
    else {
        users->hide();
        whisper->hide();
    }
}

void MainWindow::on_connectAction_triggered()
{
    chatZone->append(tr("<strong>Connecting to ")+settings->getHost()+":"+QString::number(settings->getPort())+tr("...</strong>"));
    socket->abort(); // Closing old connexions
    socket->connectToHost(settings->getHost(), settings->getPort());
}

void MainWindow::on_disconnectAction_triggered()
{
    socket->disconnectFromHost();
}

void MainWindow::on_message_returnPressed()
{
    appendData("<span style=\"color:black;\">"+settings->getNickname()+": "+message->text()+"</span>");
    dataSend(CS_PUBMSG, message->text());
    message->clear();
    message->setFocus();
}
void MainWindow::on_whisper_returnPressed()
{
    appendData("<span style=\"color:blue;\"><em>("+tr("to: ")+users->currentText()+") "+whisper->text()+"</em></span>");
    dataSend(CS_PRIVMSG, users->currentText()+":"+whisper->text());
    whisper->clear();
    whisper->setFocus();
}



void MainWindow::dataSend(quint16 msgCode, QString msgToSend)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << (quint16) msgCode;
    out << msgToSend;
    out.device()->seek(0);
    out << (quint16) (packet.size()-sizeof(quint16));

    socket->write(packet);

}

void MainWindow::dataRecv()
{
    forever {
    QDataStream in(socket);
    if (messageSize == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
             return;
        in >> messageSize;
    }

    if (socket->bytesAvailable() < messageSize)
        return; // we wait for the end of the message

    quint16 messageCode;
    in >> messageCode;

    QString messageText;
    in >> messageText;

    dataHandler(messageCode,messageText);

    // we reset the messageSize to 0, waiting for next data
    messageSize = 0;
}
}

void MainWindow::dataHandler(quint16 dataCode, QString data)
{
    QStringList split;
    QString sender;
    switch (dataCode) {
    case SC_PUBMSG:
        split=data.split(":");
        sender=split[0];
        split.removeAt(0);
        data = "<span style=\"color:black;\">"+sender+": "+split.join(":")+"</span>";
        appendData(data);
        break;
    case SC_NICKINUSE:
        data = "<span style=\"color:red;\"><strong>"+tr("This nick is already used. Please retry with another one")+"</strong></span>";
        appendData(data);
        socket->disconnectFromHost(); // FIXME: should be managed by the server
        break;
    case SC_ERRONEOUSNICK:
        data = "<span style=\"color:red;\"><strong>"+tr("Erroneous nickname. Please retry with another one")+"</strong></span>";
        appendData(data);
        socket->disconnectFromHost(); // FIXME: should be managed by the server
        break;
    case SC_EVENT:
        data = "<em>"+data+"</em>";
        appendData(data);
        break;
    case SC_JOIN:
        dataSend(CS_USERLIST);
        data = "<span style=\"color:green;\"><em>"+data+tr(" just log in")+"</em></span>";
        appendData(data);
        break;
    case SC_PART:
        dataSend(CS_USERLIST);
        data = "<span style=\"color:brown;\"><em>"+data+tr(" has quit")+"</em></span>";
        appendData(data);
        break;
    case SC_USERLIST:
        users->clear();
        if (!data.isEmpty()) {
            whisper->setEnabled(true);
            foreach(QString nick, data.split(":")) {
                users->addItem(nick);
            }
        }
        else {
            whisper->setEnabled(false);
            users->setEnabled(false);
        }

        break;
    case SC_PRIVMSG:
        split = data.split(":");
        sender=split[0];
        split.removeAt(0);
        data = "<span style=\"color:blue;\"><em>"+sender+": "+split.join(":")+"</em></span>";
        appendData(data);
        break;
    default:
        appendData(tr("Unknown message received"));
    }
}

void MainWindow::appendData(QString data)
{
    if (displayTimeAction->isChecked()) data = QDateTime::currentDateTime().toString("[hh:mm:ss] ")+data;
    chatZone->append(data);
}

void MainWindow::clientConnect()
{
    dataSend(CS_AUTH,settings->getNickname());
    chatZone->append(tr("<strong>Connection successful</strong>"));
    connectAction->setEnabled(false);
    disconnectAction->setEnabled(true);
    message->setEnabled(true);
    whisper->setEnabled(true);
    users->setEnabled(true);
}

void MainWindow::clientDisconnect()
{
    chatZone->append(tr("<strong>Disconnected from the server</strong>"));
    connectAction->setEnabled(true);
    disconnectAction->setEnabled(false);
    message->setEnabled(false);
    whisper->setEnabled(false);
    users->clear();
    users->setEnabled(false);
}

void MainWindow::socketError(QAbstractSocket::SocketError erreur)
{
    switch(erreur)
    {
        case QAbstractSocket::HostNotFoundError:
            chatZone->append(tr("<strong>ERROR : host not found</strong>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            chatZone->append(tr("<strong>ERROR : connection refused. Is the server launched?</strong>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            chatZone->append(tr("<strong>ERROR : connection closed by remote host</strong>"));
            break;
        default:
            chatZone->append(tr("<strong>ERROR : ") + socket->errorString() + tr("</strong>"));
    }
}

void MainWindow::showRenderZone()
{
    renderZone->show();
}
