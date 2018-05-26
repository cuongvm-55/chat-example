#include "NetworkManager.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket();

    m_socket->setSocketOption(QAbstractSocket::SocketOption::KeepAliveOption, 1);
    QObject::connect(m_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    QObject::connect(m_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    QObject::connect(m_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketErrored(QAbstractSocket::SocketError)));

    m_serverPort = 0;
}

NetworkManager::~NetworkManager()
{
    if( m_socket )
    {
        m_socket->close();
        delete m_socket;
    }
}

bool NetworkManager::connect(const QUrl &url, int port)
{
    m_serverUrl = url;
    m_serverPort = port;

    // connect to server
    m_socket->connectToHost(url.url(), port);
    return true;
}

void NetworkManager::disconnect()
{
    m_socket->disconnectFromHost();
}

bool NetworkManager::sendData(const QString &data)
{
    quint64 written = m_socket->write(data.toUtf8());
    return written > 0;
}

void NetworkManager::handleIncomingData(const QByteArray &data)
{
    emit dataReceived(data);
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

void NetworkManager::onSocketConnected()
{
    qDebug() << "socket connected";
    emit connected();
}

void NetworkManager::onSocketDisconnected()
{
    qDebug() << "socket disconnected";
    emit disconnected();
}

void NetworkManager::onSocketErrored(QAbstractSocket::SocketError error)
{
    QString errString = getSocketErrorString(error);
    qDebug() << errString;

    emit networkErrored(errString);
}

void NetworkManager::onSocketReadyRead()
{
    qDebug() << "new data received";
    while ( m_socket->bytesAvailable() )
    {
        QByteArray data = m_socket->readAll();
        handleIncomingData(data);
    }
}

