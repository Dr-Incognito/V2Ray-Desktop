#include "networkrequest.h"

#include <QDateTime>
#include <QDebug>
#include <QEventLoop>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSctpSocket>
#include <QTimer>

#include "constants.h"

NetworkRequest::NetworkRequest() {}

QByteArray NetworkRequest::getNetworkResponse(QString url,
                                              const QNetworkProxy* proxy,
                                              int timeout) {
  QTimer timer;
  timer.setSingleShot(true);

  QNetworkAccessManager accessManager;
  if (proxy != nullptr) {
    accessManager.setProxy(*proxy);
  }
  QNetworkRequest request;

  request.setUrl(QUrl(url));
  request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::ManualRedirectPolicy);
  qInfo() << "Start to get url: " << url;
  QNetworkReply* networkReply = accessManager.get(request);
  QEventLoop eventLoop;
  connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
  connect(networkReply, &QNetworkReply::finished, &eventLoop,
          &QEventLoop::quit);
  if (timeout > 0) {
    timer.start(timeout);
  }
  eventLoop.exec();

  // Timeout handler
  if (timeout > 0 && !timer.isActive()) {
    disconnect(networkReply, &QNetworkReply::finished, &eventLoop,
               &QEventLoop::quit);
    networkReply->abort();
    qWarning() << "Timed out when requesting " << url;
  }
  // Network error Handler
  if (networkReply->error() != QNetworkReply::NoError) {
    qCritical() << "Error occurred during requsting " << url
                << "; Error: " << networkReply->error();
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
