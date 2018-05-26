#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpServer>
#include <QUrl>
#include <QTcpSocket>

struct Client
{
    QString name;
    QString clientId;
    QTcpSocket* socket;
    QString status;

    Client()
    {
        socket = nullptr;
    }

    ~Client()
    {
        if( socket )
        {
            socket->close();
            delete socket;
        }
    }
};

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    virtual ~NetworkManager();

    bool listen(quint16 port);
    void disconnect();

private:
    void handleIncomingData(const QString& clientId, const QByteArray& data);
    QString getSocketErrorString(QAbstractSocket::SocketError) const;

    Client* getClient(QTcpSocket*) const;
private:
    QTcpServer* m_server;
    QList<Client*> m_clients;

signals:
    void newConnection();
    void acceptError(QString);
    void dataReceived(QString, QByteArray);

public slots:
    void onNewConnection();
    void onAcceptErrored(QAbstractSocket::SocketError);

    void onClientConnected();
    void onClientDisconnected();
    void onClientDataReceived();
};

#endif // NETWORKMANAGER_H
