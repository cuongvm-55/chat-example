#include "NetworkManager.h"
#include <QDebug>
#include <QDateTime>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer();

    QObject::connect(m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    QObject::connect(m_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(onAcceptErrored(QAbstractSocket::SocketError)));
}

NetworkManager::~NetworkManager()
{
    if( m_server )
    {
        m_server->close();
        delete m_server;
    }
}

bool NetworkManager::listen(quint16 port)
{
    return m_server->listen(QHostAddress::Any, port);
}

void NetworkManager::disconnect()
{
    m_server->close();

    // clear all client
    qDeleteAll(m_clients);
}

void NetworkManager::handleIncomingData(const QString& clientId, const QByteArray &data)
{
    qDebug() << "data: " << data;
    emit dataReceived(clientId, data);
}

QString NetworkManager::getSocketErrorString(QAbstractSocket::SocketError error) const
{
    static QMap<QAbstractSocket::SocketError, QString> socketErrors = {
        {QAbstractSocket::SocketError::ConnectionRefusedError, "ConnectionRefusedError"},
        {QAbstractSocket::SocketError::RemoteHostClosedError, "RemoteHostClosedError"},
        {QAbstractSocket::SocketError::HostNotFoundError, "HostNotFoundError"},
        {QAbstractSocket::SocketError::SocketAccessError, "SocketAccessError"},
        {QAbstractSocket::SocketError::SocketResourceError, "SocketResourceError"},
        {QAbstractSocket::SocketError::SocketTimeoutError, "SocketTimeoutError"},                   /* 5 */
        {QAbstractSocket::SocketError::DatagramTooLargeError, "DatagramTooLargeError"},
        {QAbstractSocket::SocketError::NetworkError, "NetworkError"},
        {QAbstractSocket::SocketError::AddressInUseError, "AddressInUseError"},
        {QAbstractSocket::SocketError::SocketAddressNotAvailableError, "SocketAddressNotAvailableError"},
        {QAbstractSocket::SocketError::UnsupportedSocketOperationError, "UnsupportedSocketOperationError"},      /* 10 */
        {QAbstractSocket::SocketError::UnfinishedSocketOperationError, "UnfinishedSocketOperationError"},
        {QAbstractSocket::SocketError::ProxyAuthenticationRequiredError, "ProxyAuthenticationRequiredError"},
        {QAbstractSocket::SocketError::SslHandshakeFailedError, "SslHandshakeFailedError"},
        {QAbstractSocket::SocketError::ProxyConnectionRefusedError, "ProxyConnectionRefusedError"},
        {QAbstractSocket::SocketError::ProxyConnectionClosedError, "ProxyConnectionClosedError"},         /* 15 */
        {QAbstractSocket::SocketError::ProxyConnectionTimeoutError, "ProxyConnectionTimeoutError"},
        {QAbstractSocket::SocketError::ProxyNotFoundError, "ProxyNotFoundError"},
        {QAbstractSocket::SocketError::ProxyProtocolError, "ProxyProtocolError"},
        {QAbstractSocket::SocketError::OperationError, "OperationError"},
        {QAbstractSocket::SocketError::SslInternalError, "SslInternalError"},                     /* 20 */
        {QAbstractSocket::SocketError::SslInvalidUserDataError, "SslInvalidUserDataError"},
        {QAbstractSocket::SocketError::TemporaryError, "TemporaryError"}
    };

    return socketErrors.contains(error) ? socketErrors.value(error) : "unknown error";
}

Client* NetworkManager::getClient(QTcpSocket *socket) const
{
    foreach (Client* client, m_clients)
    {
        if( client->socket == socket )
        {
            return client;
        }
    }

    return nullptr;
}

void NetworkManager::onNewConnection()
{
    qDebug() << "new connection";
    QTcpSocket* socket = m_server->nextPendingConnection();
    if( socket )
    {
        QObject::connect(socket, SIGNAL(connected()), this, SLOT(onClientConnected()));
        QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
        QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(onClientDataReceived()));

        // add client to the managed list
        Client* client = new Client();
        client->name = "unknown";
        client->clientId = QString("%1.%2").arg(QString::number(QDateTime::currentMSecsSinceEpoch()), rand());
        client->socket = socket;
        m_clients.append(client);
    }
}

void NetworkManager::onAcceptErrored(QAbstractSocket::SocketError error)
{
    emit acceptError(getSocketErrorString(error));
}

void NetworkManager::onClientConnected()
{
    qDebug() << "on client connected";
}

void NetworkManager::onClientDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "on client disconnected";

    // remove client out of list
    Client* client = getClient(socket);
    if( client )
    {
        m_clients.removeOne(client);
    }
}

void NetworkManager::onClientDataReceived()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "data receive from client";

    QByteArray data;
    while( socket->bytesAvailable() )
    {
        data.append(socket->readAll());
    }

    Client* client = getClient(socket);
    if( client )
    {
        handleIncomingData(client->clientId, data);
    }
}
