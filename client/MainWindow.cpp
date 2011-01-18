#include "MainWindow.h"
#include "Protocol.h"


MainWindow::MainWindow()
{

    /*
      NOTES:
      Les segfaults se contournent en créant des attributs du genre:
        monobjet = new QObject;
      au lieu de:
        QObject *monobjet = new QObject;
      C'est étrange et j'en recherche l'explication, ainsi que la différence entre ces deux procédés
      (peut-être est-ce parce qu'ils sont déjà déclarés dans les headers ?)
    */


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

    renderZone = new QWidget;

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
    connectAction = new QAction("Se connecter",this);
    connectAction->setObjectName("connectAction");
    connectAction->setIcon(QIcon(":/img/gtk-connect.png"));

    disconnectAction = new QAction("Se déconnecter",this);
    disconnectAction->setObjectName("disconnectAction");
    disconnectAction->setIcon(QIcon(":/img/gtk-disconnect.png"));
    disconnectAction->setEnabled(false);

    quitAction = new QAction("Quitter",this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    quitAction->setIcon(QIcon(":/img/application-exit.png"));
    quitAction->setStatusTip("Quitte le programme");
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    settingsAction = new QAction("Options...",this);
    settingsAction->setIcon(QIcon(":/img/preferences-desktop.png"));
    settingsAction->setStatusTip("Options du programme");
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));

    displayWhisperAction = new QAction("Murmurer",this);
    displayWhisperAction->setObjectName("displayWhisperAction");
    displayWhisperAction->setCheckable(true);
    displayWhisperAction->setChecked(true);

    displayTimeAction = new QAction("Horodatage",this);
    displayTimeAction->setObjectName("displayTimeAction");
    displayTimeAction->setCheckable(true);
    displayTimeAction->setChecked(false);

    aboutAction = new QAction("À propos de PimakWorlds...",this);
    aboutAction->setIcon(QIcon(":/img/gtk-about.png"));
    aboutAction->setStatusTip("Informations à propos du programme");
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

}

void MainWindow::initMenus()
{
    // Menus
    QMenu *fileMenu = menuBar()->addMenu("Fichier");
    fileMenu->addAction(connectAction);
    fileMenu->addAction(disconnectAction);
    fileMenu->addAction(quitAction);
    QMenu *displayMenu = menuBar()->addMenu("Affichage");
    displayMenu->addAction(displayWhisperAction);
    displayMenu->addAction(displayTimeAction);
    QMenu *toolsMenu = menuBar()->addMenu("Outils");
    toolsMenu->addAction(settingsAction);
    QMenu *helpMenu = menuBar()->addMenu("Aide");
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
    QLabel *texte = new QLabel(tr("<h1>PimakWorlds 0.0.1a</h1><h3>koi sa march pa</h3><p>Client basé sur Qt 4.7.0</p>"));
    QPushButton *buttonClose = new QPushButton("Fermer");
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
    chatZone->append(tr("<strong>Connexion à ")+settings->getHost()+":"+QString::number(settings->getPort())+tr("...</strong>"));

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
    appendData("<span style=\"color:blue;\"><em>(to: "+users->currentText()+") "+whisper->text()+"</em></span>");
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

    QString messageRecu;
    in >> messageRecu;

    dataHandler(messageCode,messageRecu);

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
    case SC_EVENT:
        data = "<em>"+data+"</em>";
        appendData(data);
        break;
    case SC_JOIN:
        dataSend(CS_USERLIST,"");
        data = "<span style=\"color:green;\"><em>"+data+tr(" vient de se connecter")+"</em></span>";
        appendData(data);
        break;
    case SC_PART:
        dataSend(CS_USERLIST,"");
        data = "<span style=\"color:red;\"><em>"+data+tr(" vient de se déconnecter")+"</em></span>";
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
        else whisper->setEnabled(false);
        break;
    case SC_PRIVMSG:
        split = data.split(":");
        sender=split[0];
        split.removeAt(0);
        data = "<span style=\"color:blue;\"><em>"+sender+": "+split.join(":")+"</em></span>";
        appendData(data);
        break;
    default:
        appendData(tr("Message du serveur non reconnu"));
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
    chatZone->append(tr("<strong>Connexion réussie</strong>"));
    connectAction->setEnabled(false);
    disconnectAction->setEnabled(true);
    message->setEnabled(true);
    whisper->setEnabled(true);
}

void MainWindow::clientDisconnect()
{
    chatZone->append(tr("<strong>Déconnecté du serveur</strong>"));
    connectAction->setEnabled(true);
    disconnectAction->setEnabled(false);
    message->setEnabled(false);
    whisper->setEnabled(false);
    users->clear();
}

void MainWindow::socketError(QAbstractSocket::SocketError erreur)
{
    switch(erreur)
    {
        case QAbstractSocket::HostNotFoundError:
            chatZone->append(tr("<strong>ERREUR : serveur non trouvé.</strong>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            chatZone->append(tr("<strong>ERREUR : connexion refusée. Le serveur est-il bien lancé ?</strong>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            chatZone->append(tr("<strong>ERREUR : le serveur a coupé la connexion.</strong>"));
            break;
        default:
            chatZone->append(tr("<strong>ERREUR : ") + socket->errorString() + tr("</strong>"));
    }
}
