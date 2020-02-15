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

  void appVersionReady(QString appVersion);
  void v2RayCoreVersionReady(QString v2RayCoreVersion);
  void operatingSystemReady(QString operatingSystem);
  void v2RayCoreStatusReady(QString v2RayCoreStatus);
  void v2RayRunningStatusChanging(bool isChanged);
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

 public slots:
  void getAppVersion();
  void getV2RayCoreVersion();
  void getOperatingSystem();
  void getV2RayCoreStatus();
  void setV2RayCoreRunning(bool expectedRunning);
  void getNetworkStatus();
  void getAppConfig();
  void setAppConfig(QString configString);
  void setAutoStart(bool autoStart);
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
  void addServerConfigFile(QString configFilePath);
  void editServer(QString serverName, QString protocol, QString configString);
  void updateSubscriptionServers(QString subsriptionUrl = "");
  void removeServer(QString serverName);
  void removeSubscriptionServers(QString subscriptionUrl);
  void scanQrCodeScreen();
  void copyToClipboard(QString text);
  bool retranslate(QString language = "");

 private slots:
  void returnServerLatency(QMap<QString, QVariant> latency);
  void returnGfwList(QByteArray gfwList);
  void returnNetworkAccessiblity(QMap<QString, bool> accessible);
  void addSubscriptionServers(QString subsriptionServers,
                              QString subsriptionUrl = "");
  void returnLogs(QString logs);

 private:
  V2RayCore& v2ray;
  Configurator& configurator;
  QJsonObject serverLatency;
  PacServer pacServer;
  AppProxyWorker* worker = new AppProxyWorker();
  QThread workerThread;
  QTranslator translator;

  QJsonObject getPrettyV2RayConfig(const QJsonObject& serverConfig);
  QJsonObject getV2RayServerFromUrl(const QString& serverUrl,
                                    const QString& subscriptionUrl = "");
  QJsonObject getV2RayStreamSettingsConfig(const QJsonObject& serverConfig);
  QJsonArray getRandomUserAgents(int n);
  QJsonObject getPrettyShadowsocksConfig(const QJsonObject& serverConfig);
  QJsonObject getShadowsocksServerFromUrl(QString serverUrl,
                                          const QString& subscriptionUrl = "");
  QNetworkProxy getQProxy();
  QStringList getAppConfigErrors(const QJsonObject& appConfig);
  QStringList getShadowsocksServerConfigErrors(const QJsonObject& serverConfig);
  bool isIpAddrValid(const QString& ipAddr);
  bool isDomainNameValid(const QString& domainName);
  bool isUrlValid(const QString& url);
};

#endif  // APPPROXY_H
