#include "appproxyworker.h"
#include "networkrequest.h"

#include <QDebug>
#include <QDir>
#include <QFile>

#include "constants.h"
#include "utility.h"

AppProxyWorker::AppProxyWorker(QObject* parent) : QObject(parent) {}

void AppProxyWorker::getServerLatency(QJsonArray servers) {
  QMap<QString, QVariant> serverLatency;
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    QString serverName = server["name"].toString();
    int latency        = NetworkRequest::getLatency(server["server"].toString(),
                                             server["port"].toInt());
    serverLatency[serverName] = latency;
  }
  emit serverLatencyReady(serverLatency);
}

void AppProxyWorker::getGfwList(QString gfwListUrl, QNetworkProxy proxy) {
  QNetworkProxy* p =
    proxy.type() == QNetworkProxy::ProxyType::NoProxy ? nullptr : &proxy;
  QString gfwList = NetworkRequest::getNetworkResponse(gfwListUrl, p);
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
      QString("http://www.%1").arg(url), p, HTTP_GET_TIMEOUT);
    accessible[url] = response.size() > 0;
  }
  emit urlAccessibilityReady(accessible);
}

void AppProxyWorker::getSubscriptionServers(QString subscriptionUrl,
                                            QNetworkProxy proxy) {
  QNetworkProxy* p =
    proxy.type() == QNetworkProxy::ProxyType::NoProxy ? nullptr : &proxy;
  QByteArray response = NetworkRequest::getNetworkResponse(subscriptionUrl, p);
  QByteArray subscriptionServers = QByteArray::fromBase64(response);
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
      logs.append(Utility::formatV2RayLog(*itr));
    }
    v2RayLogFile.close();
  }
  // Sort logs by timestamp
  logs.sort();
  std::reverse(logs.begin(), logs.end());

  emit logsReady(logs.join('\n'));
}

void AppProxyWorker::getLatestRelease(QString name,
                                      QString releaseUrl,
                                      QNetworkProxy proxy) {
  QNetworkProxy* p =
    proxy.type() == QNetworkProxy::ProxyType::NoProxy ? nullptr : &proxy;
  QString latestRelease = Utility::getLatestRelease(releaseUrl, p);

  emit latestReleaseReady(name, latestRelease);
}
