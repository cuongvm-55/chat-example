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
    QUrl url;
    url.setUrl("localhost");
    manager->connect(url, 987);

    return app.exec();
}
