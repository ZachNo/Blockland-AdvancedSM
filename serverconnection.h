#ifndef SERVERCONNECTION
#define SERVERCONNECTION

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

class MainWindow;

class ServerConnection : public QObject
{
    Q_OBJECT
public:
    ServerConnection(MainWindow *m);
    ~ServerConnection();
    void sendCommand(QString command);
public slots:
    void reconnect();
private slots:
    void onMessageReceived();
    void onConnected();
    void onError(QAbstractSocket::SocketError);
private:
    QTcpSocket *tcp;
    MainWindow *main;
};

#endif // SERVERCONNECTION

