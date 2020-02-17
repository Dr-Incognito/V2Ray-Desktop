#ifndef SERVERCONFIGHELPER_H
#define SERVERCONFIGHELPER_H

#include <QObject>

class ServerConfigHelper : public QObject {
  Q_OBJECT
 public:
  explicit ServerConfigHelper(QObject *parent = nullptr) : QObject(parent) {}
  enum class Protocol { VMESS, SHADOWSOCKS, UNKNOWN };

  static Protocol getProtocol(QString protocol);
  static QStringList getServerConfigErrors(
    Protocol protocol,
    const QJsonObject &serverConfig,
    const QString *pServerName = nullptr);
  static QJsonObject getPrettyServerConfig(Protocol protocol,
                                           const QJsonObject &serverConfig);
  static QJsonObject getServerConfigFromUrl(Protocol protocol,
                                            const QString &serverUrl,
                                            const QString &subscriptionUrl);
  static QList<QJsonObject> getServerConfigFromV2RayConfig(
    const QJsonObject &config);
  static QList<QJsonObject> getServerConfigFromShadowsocksQt5Config(
    const QJsonObject &config);

 private:
  static QStringList getV2RayServerConfigErrors(
    const QJsonObject &serverConfig, const QString *pServerName = nullptr);
  static QStringList getV2RayStreamSettingsErrors(
    const QJsonObject &serverConfig, const QString &network);
  static QString getServerNameError(const QJsonObject &serverConfig,
                                    const QString *pServerName = nullptr);
  static QJsonObject getV2RayServerSettingsFromConfig(
    const QJsonObject &settings);
  static QJsonObject getV2RayStreamSettingsFromConfig(
    const QJsonObject &transport, const QJsonObject &streamSettings);
  static QJsonObject getPrettyV2RayConfig(const QJsonObject &serverConfig);
  static QJsonObject getV2RayStreamSettingsConfig(
    const QJsonObject &serverConfig);
  static QJsonArray getRandomUserAgents(int n);
  static QStringList getShadowsocksServerConfigErrors(
    const QJsonObject &serverConfig, const QString *pServerName = nullptr);
  static QJsonObject getPrettyShadowsocksConfig(
    const QJsonObject &serverConfig);
  static QJsonObject getV2RayServerConfigFromUrl(
    const QString &server, const QString &subscriptionUrl);
  static QJsonObject getShadowsocksServerConfigFromUrl(
    QString serverUrl, const QString &subscriptionUrl);
  static QJsonObject getShadowsocksPlugins(const QString &pluginString);
};

#endif  // SERVERCONFIGHELPER_H
