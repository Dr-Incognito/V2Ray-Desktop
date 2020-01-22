#ifndef APPPROXY_H
#define APPPROXY_H

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QString>

#include "configurator.h"
#include "v2raycore.h"

class AppProxy : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(AppProxy)

 public:
  AppProxy(QObject* parent = 0);

 signals:
  void appVersionReady(QString appVersion);
  void v2RayCoreVersionReady(QString v2RayCoreVersion);
  void operatingSystemReady(QString operatingSystem);
  void v2RayCoreStatusReady(QString v2RayCoreStatus);
  void v2RayRunningStatusChanging(bool isChanged);
  void appConfigReady(QString appConfig);
  void appConfigError(QString errorMessage);
  void appConfigChanged();
  void logsReady(QString logs);
  void serversReady(QString servers);
  void addServerError(QString errorMessage);
  void serversAdded(QString addServerMethod);
  void serversChanged();

 public slots:
  QString getAppVersion();
  QString getV2RayCoreVersion();
  QString getOperatingSystem();
  QString getV2RayCoreStatus();
  bool setV2RayCoreRunning(bool expectedRunning);
  QJsonObject getAppConfig();
  void saveAppConfig(QString configString);
  QString getLogs();
  void clearLogs();
  QJsonArray getServers();
  void setServerConnection(QString serverName, bool connected);
  void addV2RayServer(QString configString);
  void addShadowsocksServer(QString configString);
  void addSubscriptionUrl(QString subsriptionUrl);
  void addServerConfigFile(QString configFilePath);
  void editServer(QString serverName, QString configString);
  void removeServer(QString serverName);

 private:
  V2RayCore& v2ray;
  Configurator& configurator;
  QJsonObject getPrettyV2RayConfig(const QJsonObject& serverConfig);
  QJsonObject getV2RayStreamSettingsConfig(const QJsonObject& serverConfig);
  QJsonArray getRandomUserAgents(int n);
  QJsonObject getPrettyShadowsocksConfig(const QJsonObject& serverConfig);
};

#endif  // APPPROXY_H
