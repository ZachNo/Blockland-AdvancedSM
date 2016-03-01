#include "serverConnection.h"
#include "mainwindow.h"

ServerConnection::ServerConnection(MainWindow *m)
{
    main = m;

    tcp = new QTcpSocket(this);

    connect(tcp, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(tcp, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

ServerConnection::~ServerConnection()
{
    delete tcp;
}

void ServerConnection::onConnected()
{
    main->updateStatus("Successfully connected to local Blockland Server.");
}

void ServerConnection::onError(QAbstractSocket::SocketError)
{
    main->updateStatus("Error while attemping connection to local Blockland server:\n | ");
    main->updateStatus(tcp->errorString());
}

void ServerConnection::onMessageReceived()
{
    QByteArray data = tcp->readLine();
    QString stringData(data);
    if(stringData.startsWith("PLAYERS "))
        main->updatePlayers(stringData.remove("PLAYERS "));
    return;
}

void ServerConnection::reconnect()
{
    tcp->abort();
    tcp->connectToHost(QHostAddress::LocalHost, 5123);
    main->updateStatus("Attempting reconnection with local Blockland Server.");
}

void ServerConnection::sendCommand(QString command)
{
    tcp->write(command.append("\n").toStdString().c_str());
}

