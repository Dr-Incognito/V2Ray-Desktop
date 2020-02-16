#ifndef SERVERCONFIGHELPER_H
#define SERVERCONFIGHELPER_H

#include <QObject>

class ServerConfigHelper : public QObject {
  Q_OBJECT
 public:
  explicit ServerConfigHelper(QObject *parent = nullptr) : QObject(parent) {}
  enum class Protocol { VMESS, SHADOWSOCKS, UNKNOWN };

  static Protocol getProtocol(QString protocol);
  static QStringList getServerConfigErrors(const QJsonObject &serverConfig,
                                           Protocol protocol);
  static QJsonObject getPrettyServerConfig(const QJsonObject &serverConfig,
                                           Protocol protocol);
  static QJsonObject getServerConfigFromUrl(const QString &serverUrl,
                                            const QString &subscriptionUrl,
                                            Protocol protocol);
  static QList<QJsonObject> getServerConfigFromShadowsocksQt5Config(
    const QJsonObject &config);

 private:
  static QStringList getV2RayServerConfigErrors(
    const QJsonObject &serverConfig);
  static QStringList getV2RayStreamSettingsErrors(
    const QJsonObject &serverConfig, const QString &network);
  static QJsonObject getPrettyV2RayConfig(const QJsonObject &serverConfig);
  static QJsonObject getV2RayStreamSettingsConfig(
    const QJsonObject &serverConfig);
  static QJsonArray getRandomUserAgents(int n);
  static QStringList getShadowsocksServerConfigErrors(
    const QJsonObject &serverConfig);
  static QJsonObject getPrettyShadowsocksConfig(
    const QJsonObject &serverConfig);
  static QJsonObject getV2RayServerConfigFromUrl(
    const QString &server, const QString &subscriptionUrl);
  static QJsonObject getShadowsocksServerConfigFromUrl(
    QString serverUrl, const QString &subscriptionUrl);
  static QJsonObject getShadowsocksPlugins(const QString &pluginString);
};

#endif  // SERVERCONFIGHELPER_H
