#include "pacserver.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QIODevice>

#include "configurator.h"

PacServer::PacServer(QObject* parent) : QObject(parent) {
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

  QFile pacFile(Configurator::getPacFilePath());
  QTextStream outputStream(clientSocket);
  outputStream.setAutoDetectUnicode(true);
  outputStream
    << "HTTP/1.1 200 Ok\r\n"
       "Content-Type: text/x-ns-proxy-autoconfig; charset=\"utf-8\"\r\n"
       "Connection: keep-alive\r\n\r\n";
  if (pacFile.exists() && pacFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    outputStream << pacFile.readAll();
  } else {
    outputStream << "PAC File does not exist.";
  }
  clientSocket->waitForBytesWritten();
  clientSocket->close();
}
