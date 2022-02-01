#ifndef APPPROXYWORKER_H
#define APPPROXYWORKER_H

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QNetworkProxy>
#include <QObject>
#include <QString>
#include <QVariant>

class AppProxyWorker : public QObject {
  Q_OBJECT
 public:
  explicit AppProxyWorker(QObject* parent = nullptr);

 public slots:
  void getServerLatency(QJsonArray servers);
  void getGfwList(QString gfwListUrl, QNetworkProxy proxy);
  void getUrlAccessibility(QMap<QString, bool> urls, QNetworkProxy proxy);
  void getSubscriptionServers(QString url, QNetworkProxy proxy);
  void getLogs(QString appLogFilePath, QString v2RayLogFilePath);
  void getLatestRelease(QString name, QString releaseUrl, QNetworkProxy proxy);

 signals:
  void serverLatencyReady(QMap<QString, QVariant> latency);
  void gfwListReady(QString gfwList);
  void urlAccessibilityReady(QMap<QString, bool> accessible);
  void subscriptionServersReady(QString subscriptionServers,
                                QString subscriptionUrl);
  void logsReady(QString logs);
  void latestReleaseReady(QString name, QString version);
};

#endif  // APPPROXYWORKER_H
