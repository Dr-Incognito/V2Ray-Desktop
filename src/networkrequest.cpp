#include "networkrequest.h"

#include <QDateTime>
#include <QDebug>
#include <QEventLoop>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSctpSocket>

#include "constants.h"

NetworkRequest::NetworkRequest() {}

QByteArray NetworkRequest::getUrl(QString url) {
  QNetworkAccessManager accessManager;
  QNetworkRequest request;

  request.setUrl(QUrl(url));
  request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
  QNetworkReply* networkReply = accessManager.get(request);
  QEventLoop eventLoop;
  connect(networkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
  eventLoop.exec();

  if (networkReply->error() != QNetworkReply::NoError) {
    return QByteArray();
  }

  QByteArray responseBytes = networkReply->readAll();
  networkReply->deleteLater();
  networkReply->manager()->deleteLater();
  return responseBytes;
}

int NetworkRequest::getLatency(QString host, int port) {
  QTcpSocket socket;
  QDateTime time = QDateTime::currentDateTime();
  socket.connectToHost(host, port);
  socket.waitForConnected(TCP_PING_TIMEOUT);
  int timeEclipsed = time.msecsTo(QDateTime::currentDateTime());
  return timeEclipsed >= TCP_PING_TIMEOUT ? -1 : timeEclipsed;
}
