#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDebug>
#include "v2raycore.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
  QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreated, &app,
    [url](QObject *obj, const QUrl &objUrl) {
      if (!obj && url == objUrl) QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);
  engine.load(url);

  // Load application config

  // Start V2Ray core
  V2RayCore v2ray;
  v2ray.start();

  return app.exec();
}
