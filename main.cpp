
#include <QGuiApplication>
#include <QtQml>
#include <QQuickStyle>

// QuickQanava headers
#include <QuickQanava.h>

#include "applicationmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine engine;
    engine.addPluginPath(QStringLiteral("./QuickQanava/src")); // Necessary only for development when plugin is not installed to QTDIR/qml
    QuickQanava::initialize(&engine);
    qmlRegisterType<CustomGraph>("MyModule", 1, 0, "CustomGraph");

//    engine.load(QUrl("qrc:/app.qml"));
    ApplicationManager* app_man = new ApplicationManager(engine);
//    engine.rootContext()->setContextProperty("app_man", app_man);
    return app.exec();
}
