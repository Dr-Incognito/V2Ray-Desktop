#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTextStream>
#include <QtGlobal>

#include "appproxy.h"
#include "constants.h"

void messageHandler(QtMsgType msgType,
                    const QMessageLogContext &context,
                    const QString &msg) {
  Q_UNUSED(context);

  QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
  QString logMessage("%1 %2 v2ray-desktop: %3");
  QString msgTypeStr;
  switch (msgType) {
    case QtDebugMsg: msgTypeStr = "[Debug]"; break;
    case QtInfoMsg: msgTypeStr = "[Info]"; break;
    case QtWarningMsg: msgTypeStr = "[Warning]"; break;
    case QtCriticalMsg: msgTypeStr = "[Critical]"; break;
    case QtFatalMsg: msgTypeStr = "[Fatal]"; break;
    default: break;
  }
  QFile logFile(LOG_FILE_PATH);
  logFile.open(QIODevice::WriteOnly | QIODevice::Append);
  QTextStream logTextStream(&logFile);
  logTextStream << logMessage.arg(dt, msgTypeStr, msg) << endl;
}

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);
  qmlRegisterType<AppProxy>("com.v2ray.desktop.AppProxy", APP_VERSION_MAJOR,
                            APP_VERSION_MINOR, "AppProxy");

  // Set up logging
  qInstallMessageHandler(messageHandler);

  // Set up the application
  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
  QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreated, &app,
    [url](QObject *obj, const QUrl &objUrl) {
      if (!obj && url == objUrl) {
        QCoreApplication::exit(-1);
      }
    },
    Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
