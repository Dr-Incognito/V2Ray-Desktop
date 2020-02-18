#ifndef APPPROXY_H
#define APPPROXY_H

#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QThread>
#include <QTranslator>

#include "appproxyworker.h"
#include "configurator.h"
#include "pacserver.h"
#include "serverconfighelper.h"
#include "v2raycore.h"

class AppProxy : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(AppProxy)

 public:
  AppProxy(QObject* parent = 0);
  ~AppProxy();

 signals:
  void getServerLatencyStarted(QJsonArray servers);
  void getGfwListStarted(QString gfwListUrl, QNetworkProxy proxy);
  void getNetworkStatusStarted(QMap<QString, bool> urls, QNetworkProxy proxy);
  void getSubscriptionServersStarted(QString url, QNetworkProxy proxy);
  void getLogsStarted(QString appLogFilePath, QString v2RayLogFilePath);
  void getLatestReleaseStarted(QString name,
                               QString releaseUrl,
                               QNetworkProxy proxy);
  void upgradeStarted(QString name,
                      QString assetsUrl,
                      QString outputFolderPath,
                      QNetworkProxy proxy);

  void appVersionReady(QString appVersion);
  void v2RayCoreVersionReady(QString v2RayCoreVersion);
  void operatingSystemReady(QString operatingSystem);
  void v2RayCoreStatusReady(bool isRunning);
  void networkStatusReady(QString networkStatus);
  void proxySettingsReady(QString proxySettings);
  void appConfigReady(QString appConfig);
  void appConfigError(QString errorMessage);
  void appConfigChanged();
  void logsReady(QString logs);
  void proxyModeReady(QString proxyMode);
  void proxyModeChanged(QString proxyMode);
  void gfwListUpdated(QString gfwListUpdateTime);
  void serversReady(QString servers);
  void serverDInfoReady(QString server);
  void serverLatencyReady(QString latency);
  void serverConfigError(QString errorMessage);
  void serverConnectivityChanged(QString serverName, bool connected);
  void serverChanged(QString serverName, QString serverConfig);
  void serverRemoved(QString serverName);
  void serversChanged();
  void latestReleaseReady(QString name, QString version);
  void latestReleaseError(QString name, QString errorMsg);
  void upgradeCompleted(QString name);
  void upgradeError(QString name, QString errorMsg);

 public slots:
  QString getAppVersion();
  void getV2RayCoreVersion();
  void getOperatingSystem();
  void getV2RayCoreStatus();
  void setV2RayCoreRunning(bool expectedRunning);
  void getNetworkStatus();
  void getAppConfig();
  void setAppConfig(QString configString);
  void setSystemProxyMode(QString proxyMode = "");
  void getProxySettings();
  void setGfwListUrl(QString gfwListUrl);
  void updateGfwList(QString gfwListUrl);
  void getLogs();
  void clearLogs();
  void getServers();
  void getServer(QString serverName, bool forDuplicate = false);
  void getServerLatency(QString serverName = "");
  void setServerConnection(QString serverName, bool connected);
  void addV2RayServer(QString configString);
  void addShadowsocksServer(QString configString);
  void addServerConfigFile(QString configFilePath, QString configFileType);
  void editServer(QString serverName, QString protocol, QString configString);
  void addSubscriptionUrl(QString subsriptionUrl);
  void updateSubscriptionServers(QString subsriptionUrl = "");
  void removeServer(QString serverName);
  void removeSubscriptionServers(QString subscriptionUrl);
  void scanQrCodeScreen();
  void copyToClipboard(QString text);
  bool retranslate(QString language = "");
  void getLatestRelease(QString name);
  void upgradeDependency(QString name, QString version);

 private slots:
  void returnServerLatency(QMap<QString, QVariant> latency);
  void returnGfwList(QByteArray gfwList);
  void returnNetworkAccessiblity(QMap<QString, bool> accessible);
  void addSubscriptionServers(QString subsriptionServers,
                              QString subsriptionUrl = "");
  void returnLogs(QString logs);
  void returnLatestRelease(QString name, QString version);
  void replaceDependency(QString name,
                         QString outputFilePath,
                         QString errorMsg);

 private:
  V2RayCore& v2ray;
  QJsonObject serverLatency;
  Configurator& configurator;
  PacServer pacServer;
  QMap<QString, QMap<QString, QVariant>> latestVersion;

  AppProxyWorker* worker = new AppProxyWorker();
  QThread workerThread;
  QTranslator translator;

  QNetworkProxy getQProxy();
  void setAutoStart(bool autoStart);
  QStringList getAppConfigErrors(const QJsonObject& appConfig);
};

#endif  // APPPROXY_H
