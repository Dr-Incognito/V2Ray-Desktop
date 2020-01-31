#include "appproxyworker.h"
#include "networkrequest.h"

#include <QDebug>

#include "constants.h"

AppProxyWorker::AppProxyWorker(QObject* parent) : QObject(parent) {}

void AppProxyWorker::getServerLatency(QJsonArray servers) {
  QMap<QString, QVariant> serverLatency;
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    QString serverName = server["serverName"].toString();
    int latency =
      NetworkRequest::getLatency(getServerAddr(server), getServerPort(server));
    serverLatency[serverName] = latency;
  }
  emit serverLatencyReady(serverLatency);
}

QString AppProxyWorker::getServerAddr(QJsonObject server) {
  QString protocol     = server["protocol"].toString();
  QString keyName      = protocol == "vmess" ? "vnext" : "servers";
  QJsonObject settings = server["settings"].toObject();
  QJsonArray servers   = settings[keyName].toArray();
  QJsonObject _server  = servers.at(0).toObject();
  return _server["address"].toString();
}

int AppProxyWorker::getServerPort(QJsonObject server) {
  QString protocol     = server["protocol"].toString();
  QString keyName      = protocol == "vmess" ? "vnext" : "servers";
  QJsonObject settings = server["settings"].toObject();
  QJsonArray servers   = settings[keyName].toArray();
  QJsonObject _server  = servers.at(0).toObject();
  return _server["port"].toInt();
}

void AppProxyWorker::getGfwList(QString gfwListUrl) {
  QByteArray gfwList =
    QByteArray::fromBase64(NetworkRequest::getNetworkResponse(gfwListUrl));
  qRegisterMetaType<QByteArray>("QByteArray");
  emit gfwListReady(gfwList);
}

void AppProxyWorker::getUrlAccessibility(QMap<QString, bool> urls,
                                         QNetworkProxy proxy) {
  // Ref:
  // https://stackoverflow.com/questions/8517853/iterating-over-a-qmap-with-for
  // Note: Convert to StdMap for better performance
  QMap<QString, bool> accessible;
  for (auto l : urls.toStdMap()) {
    QString url         = l.first;
    bool useProxy       = l.second;
    QNetworkProxy* p    = useProxy ? &proxy : nullptr;
    QByteArray response = NetworkRequest::getNetworkResponse(
      QString("https://www.%1").arg(url), p, HTTP_GET_TIMEOUT);
    accessible[url] = response.size() > 0;
  }
  emit urlAccessibilityReady(accessible);
}

void AppProxyWorker::getSubscriptionServers(QString subscriptionUrl) {
  QByteArray subscriptionServers =
    QByteArray::fromBase64(NetworkRequest::getNetworkResponse(subscriptionUrl));
  emit subscriptionServersReady(subscriptionUrl, subscriptionServers);
}
