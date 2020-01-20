#include "httprequest.h"

#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

HttpRequest::HttpRequest() {}

QByteArray HttpRequest::get(QString url) {
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
