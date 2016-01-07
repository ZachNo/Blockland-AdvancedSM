#include "currentPlayers.h"
#include "sleep.h"

Receiver::Receiver(QStandardItemModel *moo, QStatusBar *boop, QString *lText, QObject *parent) : QObject(parent), model(moo), status(boop)
{
    logText = lText;

    tcp = new QTcpSocket(this);

    connect(tcp, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(tcp, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    tcp->connectToHost(QHostAddress::LocalHost, 5000);

    logText->append("\nAttempting connection to local Blockland server.");
}

void Receiver::onConnected()
{
    status->showMessage(tr("Connected to Blockland Server..."));
    logText->append("\nSuccessfully connected to local Blockland Server.");
}

void Receiver::onError(QAbstractSocket::SocketError)
{
    status->showMessage(tcp->errorString());
    logText->append("\nError while attemping connection to local Blockland server:\n | ");
    logText->append(tcp->errorString());
}

void Receiver::onMessageReceived()
{
    model->setRowCount(0);

    QByteArray data = tcp->readLine();
    if(data.isEmpty())
        return;
    QString input(data);
    QStringList Players = input.split('|');
    for(int i = 0; i < Players.size(); i++)
    {
        QStringList moo = Players.at(i).split('\t');
        QList<QStandardItem *> items;
        items.append(new QStandardItem(moo.at(0)));
        items.append(new QStandardItem(moo.at(1)));
        items.append(new QStandardItem(moo.at(2)));
        model->appendRow(items);
    }
}

void Receiver::reconnect()
{
    tcp->abort();
    tcp->connectToHost(QHostAddress::LocalHost, 5000);
    status->showMessage(tr("Trying to reconnect"));
    logText->append("\nAttempting reconnection with local Blockland Server.");
}
