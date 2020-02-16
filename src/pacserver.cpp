#include "pacserver.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QIODevice>

#include "constants.h"

PacServer::PacServer(QObject* parent)
  : QObject(parent), configurator(Configurator::getInstance()) {
  server = new QTcpServer(this);
  connect(server, &QTcpServer::newConnection, this,
          &PacServer::onNewConnection);
}

PacServer::~PacServer() { delete server; }

bool PacServer::start(QString addr, int port) {
  if (!server->listen(QHostAddress(addr), port)) {
    qCritical() << QString("Failed to start server at %1:%2")
                     .arg(addr, QString::number(port));
    return false;
  }
  bool isStarted = server->isListening();
  if (isStarted) {
    qInfo()
      << QString("Start PAC server at %1:%2").arg(addr, QString::number(port));
  }
  return isStarted;
}

bool PacServer::stop() {
  server->close();
  bool isStopped = server->isListening();
  if (isStopped) {
    qInfo() << "PAC Server stopped";
  } else {
    qWarning() << "Failed to stop PAC server";
  }
  return isStopped;
}

bool PacServer::isRunning() { return server->isListening(); }

void PacServer::onNewConnection() {
  QTcpSocket* clientSocket = server->nextPendingConnection();
  while (!(clientSocket->waitForReadyRead()))
    ;
  connect(clientSocket, &QAbstractSocket::disconnected, clientSocket,
          &QObject::deleteLater);

  QTextStream outputStream(clientSocket);
  outputStream.setAutoDetectUnicode(true);
  outputStream
    << "HTTP/1.1 200 Ok\r\n"
       "Content-Type: text/x-ns-proxy-autoconfig; charset=\"utf-8\"\r\n"
       "Connection: keep-alive\r\n\r\n";

  QFile pacFile(":/misc/tpl-proxy.pac");
  QString proxy = getLocalProxy();
  QString rules = getPacRules();
  if (pacFile.exists() && pacFile.open(QIODevice::ReadOnly)) {
    QString fileContent = pacFile.readAll();
    outputStream
      << fileContent.replace("__PROXY__", proxy).replace("__RULES__", rules);
    pacFile.close();
  }
  clientSocket->waitForBytesWritten();
  clientSocket->close();
}

QString PacServer::getLocalProxy() {
  QJsonObject appConfig = configurator.getAppConfig();
  return QString("%1 %2:%3")
    .arg(appConfig["serverProtocol"].toString() == "SOCKS" ? "SOCKS5" : "PROXY",
         appConfig["serverIp"].toString(),
         QString::number(appConfig["serverPort"].toInt()));
}

QString PacServer::getPacRules() {
  QJsonObject appConfig = configurator.getAppConfig();
  QStringList rules;
  QFile gfwListFile(Configurator::getGfwListFilePath());
  if (gfwListFile.exists() &&
      gfwListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QList<QByteArray> gfwList = gfwListFile.readAll().split('\n');
    for (QString gfwRule : gfwList) {
      if (gfwRule.startsWith("!") || gfwRule.startsWith("[AutoProxy") ||
          gfwRule.size() == 0) {
        continue;
      }
      rules.append(QString("\"%1\"").arg(gfwRule));
    }
    gfwListFile.close();
  }
  return QString("[%1]").arg(rules.join(",\n"));
}
