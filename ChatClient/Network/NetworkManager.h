#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpSocket>
#include <QUrl>
class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    virtual ~NetworkManager();

    // c++ mangling
    bool connect(const QUrl& url, int port);
    void disconnect();

    bool sendData(const QString& data);

private:
    void handleIncomingData(const QByteArray& data);
    QString getSocketErrorString( QAbstractSocket::SocketError) const;

private:
    QTcpSocket* m_socket;

    QUrl m_serverUrl;
    int m_serverPort;
signals:
    void connected();
    void disconnected();
    void dataReceived(QByteArray);
    void networkErrored(QString);

public slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketErrored(QAbstractSocket::SocketError);
};

#endif // NETWORKMANAGER_H
