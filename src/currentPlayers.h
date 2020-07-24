#ifndef CURRENTPLAYERS_H
#define CURRENTPLAYERS_H

#include <QtWidgets>
#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QIODevice>

class Receiver : public QObject
{
    Q_OBJECT
public:
    Receiver(QStandardItemModel *moo, QStatusBar *boop, QString *lText, QObject *parent = Q_NULLPTR);
public slots:
    void reconnect();
private slots:
    void onMessageReceived();
    void onConnected();
    void onError(QAbstractSocket::SocketError);
private:
    QTcpSocket *tcp;
    QStandardItemModel *model;
    QStatusBar *status;
    QString *logText;
};

#endif // CURRENTPLAYERS_H
