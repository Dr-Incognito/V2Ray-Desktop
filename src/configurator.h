#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

class Configurator : public QObject {
  Q_OBJECT
 public:
  static Configurator& getInstance();
  Configurator(Configurator const&) = delete;
  void operator=(Configurator const&) = delete;
  static QDir getAppConfigDir();
  static QDir getAppTempDir();
  static QString getAppFilePath();
  static QString getAppWorkingDirPath();
  static QString getV2RayInstallDirPath();
  static QString getLocaleDirPath();
  static QString getAppLogFilePath();
  static QString getAppConfigFilePath();
  static QString getV2RayLogFilePath();
  static QString getV2RayConfigFilePath();
  static QString getGfwListFilePath();
  QJsonObject getAppConfig();
  QString getLanguage();
  QJsonObject getV2RayConfig();
  void setAppConfig(QJsonObject config);
  QJsonArray getServers();
  QJsonObject getServer(QString serverName);
  QStringList getSubscriptionUrls();
  int addServer(QJsonObject serverConfig);
  int editServer(QString serverName, QJsonObject serverConfig);
  int removeServer(QString serverName);
  QMap<QString, QJsonObject> removeSubscriptionServers(QString subscriptionUrl);
  QJsonArray getConnectedServers();
  QStringList getConnectedServerNames();
  void setServerConnection(QString serverName, bool connected);
  QJsonArray getRules();
  QJsonArray getGfwListRules();
  QJsonArray getUserRules();

 private:
  Configurator();
  static QJsonObject DEFAULT_APP_CONFIG;
  QStringList connectedServerNames;
  static QString getDefaultLanguage();
  QJsonArray getPrettyDnsServers(QString dnsString);
  QStringList getAutoConnectServers();
  QJsonArray getRules(const QString& fileName);
};

#endif  // CONFIGURATOR_H
