#include "appproxyworker.h"
#include "networkrequest.h"

#include <QDebug>
#include <QFile>

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

void AppProxyWorker::getGfwList(QString gfwListUrl, QNetworkProxy proxy) {
  QNetworkProxy* p =
    proxy.type() == QNetworkProxy::ProxyType::NoProxy ? nullptr : &proxy;
  QByteArray gfwList =
    QByteArray::fromBase64(NetworkRequest::getNetworkResponse(gfwListUrl, p));
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

void AppProxyWorker::getSubscriptionServers(QString subscriptionUrl,
                                            QNetworkProxy proxy) {
  QNetworkProxy* p =
    proxy.type() == QNetworkProxy::ProxyType::NoProxy ? nullptr : &proxy;
  QByteArray subscriptionServers = QByteArray::fromBase64(
    NetworkRequest::getNetworkResponse(subscriptionUrl, p));
  emit subscriptionServersReady(subscriptionServers, subscriptionUrl);
}

void AppProxyWorker::getLogs(QString appLogFilePath, QString v2RayLogFilePath) {
  QFile appLogFile(appLogFilePath);
  QFile v2RayLogFile(v2RayLogFilePath);
  QStringList logs;
  // Read the app and V2Ray logs
  if (appLogFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QList<QByteArray> _logList = appLogFile.readAll().split('\n');
    int cnt                    = 0;
    for (auto itr = _logList.end() - 1;
         itr >= _logList.begin() && cnt <= MAX_N_LOGS; --itr, ++cnt) {
      logs.append(*itr);
    }
    appLogFile.close();
  }
  if (v2RayLogFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QList<QByteArray> _logList = v2RayLogFile.readAll().split('\n');
    int cnt                    = 0;
    for (auto itr = _logList.end() - 1;
         itr >= _logList.begin() && cnt <= MAX_N_LOGS; --itr, ++cnt) {
      logs.append(*itr);
    }
    v2RayLogFile.close();
  }
  // Sort logs by timestamp
  logs.sort();
  std::reverse(logs.begin(), logs.end());

  emit logsReady(logs.join('\n'));
}
