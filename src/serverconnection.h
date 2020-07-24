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
    bool isConnected();
public slots:
    void reconnect();
    void reconnect(int port);
private slots:
    void onMessageReceived();
    void onConnected();
    void onError(QAbstractSocket::SocketError);
private:
    QTcpSocket *tcp;
    MainWindow *main;
};

#endif // SERVERCONNECTION

