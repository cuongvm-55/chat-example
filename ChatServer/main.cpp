#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "Network/NetworkManager.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    NetworkManager* manager = new NetworkManager();
    int port = 1024;
    if ( manager->listen(port) )
    {
        qDebug() << "listening on port " << port;
    }
    else
    {
        qDebug() << "fail to listen on port " << port;
    }

    return app.exec();
}
