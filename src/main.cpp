#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "appproxy.h"
#include "constants.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  qmlRegisterType<AppProxy>("org.v2ray.desktop.AppProxy", APP_VERSION_MAJOR,
                            APP_VERSION_MINOR, "AppProxy");

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
  QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreated, &app,
    [url](QObject *obj, const QUrl &objUrl) {
      if (!obj && url == objUrl) QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);
  engine.load(url);

  // Start V2Ray core
  AppProxy appProxy;
  appProxy.setV2RayCoreRunning(true);

  return app.exec();
}
