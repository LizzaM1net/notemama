#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <iostream>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    std::cout << "QSG_RHI_PROFILE" << std::endl;
    qputenv("QSG_RHI_PROFILE", "1");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("NoteMama", "Main");

    return app.exec();
}
