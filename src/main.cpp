#include <QApplication>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QIcon>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QTextStream>
#include <QTranslator>
#include <QtGlobal>

#include "appproxy.h"
#include "constants.h"
#include "runguard.h"

void messageHandler(QtMsgType msgType,
                    const QMessageLogContext &context,
                    const QString &msg) {
  Q_UNUSED(context);
  static QString lastMsg;
  if (lastMsg == msg) {
    return;
  }
  lastMsg = msg;

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
  if (msgType != QtDebugMsg) {
    QFile logFile(Configurator::getAppLogFilePath());
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream logTextStream(&logFile);
    logTextStream << logMessage.arg(dt, msgTypeStr, msg) << Qt::endl;
    logFile.close();
  } else {
    QTextStream(stdout) << logMessage.arg(dt, msgTypeStr, msg) << Qt::endl;
  }
}

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  QApplication app(argc, argv);
  app.setApplicationName(APP_NAME);
  app.setApplicationVersion(QString("v%1.%2.%3")
                              .arg(QString::number(APP_VERSION_MAJOR),
                                   QString::number(APP_VERSION_MINOR),
                                   QString::number(APP_VERSION_PATCH)));
  app.setWindowIcon(QIcon(":/images/v2ray.ico"));
  app.setOrganizationName("V2Ray");
  app.setOrganizationDomain("v2ray.com");

  // Make sure there is no other instance running
  RunGuard runGuard(APP_NAME);
  if (!runGuard.tryToRun()) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Icon::Critical);
    msgBox.setText(
      QString(QObject::tr("There is another %1 instance running!\n"))
        .arg(APP_NAME));
    msgBox.exec();
    return 127;
  }

  // Set up QML and AppProxy
#if defined(Q_OS_WIN)
  QFont font("Microsoft YaHei", 10);
  app.setFont(font);
#endif
#if defined(Q_OS_LINUX)
  if (QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP") ==
      "KDE") {
    QQuickStyle::setStyle("fusion");
  }
#endif
  qmlRegisterSingletonType<AppProxy>(
    "com.v2ray.desktop.AppProxy", APP_VERSION_MAJOR, APP_VERSION_MINOR,
    "AppProxy", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
      Q_UNUSED(engine)
      Q_UNUSED(scriptEngine)
      AppProxy *appProxy = new AppProxy();
      return appProxy;
    });

  // Set up logging
  qInstallMessageHandler(messageHandler);

  // Set up the application
  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
  QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreated, &app,
    [url](QObject *obj, const QUrl &objUrl) {
      if (!obj && url == objUrl) {
        QApplication::exit(-1);
      }
    },
    Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
