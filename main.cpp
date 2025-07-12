#include <QApplication>  // ✅ Change this
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "backend.h"
#define QT_NO_DEBUG_OUTPUT
//#define QT_NO_INFO_OUTPUT

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // ✅ Was QGuiApplication
    /* manifest path */
    /* we need to use fota_utils path instead of manifest */ /* to do  */
    QString fota_utils_path = "/home/abdalla/learning/graduation_project/Dashboard-main/fota-utils" ;
    QString repopath  = "https://api.github.com/repos/grad-project-alex-25/fota_host/contents" ;
    QString registrypath = QString("https://api.github.com/orgs/%1/packages").arg("grad-project-alex-25") ;
    Backend backend(repopath,registrypath,fota_utils_path);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("tesla_dashboard", "Main");



    return app.exec();
    return 0 ;
    return 0 ;
}
