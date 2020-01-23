#include "configurator.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

#include "constants.h"

QJsonObject Configurator::DEFAULT_APP_CONFIG = {
  {"autoStart", DEFAULT_AUTO_START},
  {"hideWindow", DEFAULT_HIDE_WINDOW},
  {"autoUpdate", DEFAULT_AUTO_UPDATE},
  {"enableUdp", DEFAULT_ENABLE_UDP},
  {"serverProtocol", DEFAULT_SERVER_PROTOCOL},
  {"serverIp", DEFAULT_SERVER_IP},
  {"serverPort", DEFAULT_SERVER_PORT},
  {"pacPort", DEFAULT_PAC_PORT},
  {"mux", DEFAULT_MUX},
  {"dns", DEFAULT_DNS_SERVER},
  {"proxyMode", DEFAULT_PROXY_MODE},
  {"gfwListUrl", DEFAULT_GFW_LIST_URL},
  {"gfwListLastUpdated", "Never"},
  {"v2rayCoreVersion", DEFAULT_V2RAY_CORE_VERSION},
};

Configurator::Configurator() { connectedServerNames = getAutoConnectServers(); }

QStringList Configurator::getAutoConnectServers() {
  QStringList autoConnectedServers;
  QJsonArray servers = getServers();
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    if (server["autoConnect"].toBool() && server.contains("serverName")) {
      autoConnectedServers.append(server["serverName"].toString());
    }
  }
  return autoConnectedServers;
}

Configurator &Configurator::getInstance() {
  static Configurator configuratorInstance;
  return configuratorInstance;
}

QString Configurator::getAppLogFilePath() {
  return QDir(QCoreApplication::applicationDirPath())
    .filePath(APP_LOG_FILE_NAME);
}

QString Configurator::getAppConfigFilePath() {
  return QDir(QCoreApplication::applicationDirPath())
    .filePath(APP_CFG_FILE_NAME);
}

QString Configurator::getV2RayInstallDirPath() {
  return QDir(QCoreApplication::applicationDirPath())
    .filePath(V2RAY_CORE_INSTALL_DIR);
}

QString Configurator::getV2RayLogFilePath() {
  return QDir(QCoreApplication::applicationDirPath())
    .filePath(V2RAY_CORE_LOG_FILE_NAME);
}

QString Configurator::getV2RayConfigFilePath() {
  return QDir(QCoreApplication::applicationDirPath())
    .filePath(V2RAY_CORE_CFG_FILE_NAME);
}

QJsonObject Configurator::getAppConfig() {
  QJsonObject config = DEFAULT_APP_CONFIG;
  QFile appCfgFile(getAppConfigFilePath());
  if (appCfgFile.exists() &&
      appCfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QJsonDocument configDoc = QJsonDocument::fromJson(appCfgFile.readAll());
    config                  = configDoc.object();
    if (config.empty()) {
      qWarning() << "Failed to parse the app config.";
      config = DEFAULT_APP_CONFIG;
    }
  }
  return config;
}

void Configurator::setAppConfig(QJsonObject config) {
  // Load current configuration
  QJsonObject _config = getAppConfig();
  // Overwrite new values to current configuration
  for (auto itr = config.begin(); itr != config.end(); ++itr) {
    QString configName   = itr.key();
    QVariant configValue = itr.value().toVariant();
    switch (configValue.type()) {
      case QVariant::Bool: _config[configName] = configValue.toBool(); break;
      case QVariant::String:
        _config[configName] = configValue.toString();
        break;
      case QVariant::List:
        _config[configName] = configValue.toJsonArray();
        break;
      default:
        qWarning() << "Ignore unknown config item [Name=" << configName
                   << ", Type=" << configValue.type() << "]";
        break;
    }
  }
  // Save config to file
  QFile configFile(getAppConfigFilePath());
  configFile.open(QFile::WriteOnly);
  configFile.write(QJsonDocument(_config).toJson());
}

QJsonObject Configurator::getV2RayConfig() {
  QJsonObject appConfig = getAppConfig();
  QJsonObject v2RayConfig{
    {"log",
     QJsonObject{{"loglevel", "info"}, {"error", getV2RayLogFilePath()}}},
    {"inbounds",
     QJsonArray{QJsonObject{
       {"listen", appConfig["serverIp"].toString()},
       {"protocol", appConfig["serverProtocol"].toString().toLower()},
       {"port", appConfig["serverPort"].toString().toInt()},
       {"settings", QJsonObject{{"udp", appConfig["enableUdp"].toBool()}}},
     }}},
    {"outbounds", getConnectedServers()},
    {"dns", QJsonObject{{"servers",
                         getPrettyDnsServers(appConfig["dns"].toString())}}},
    {"routing",
     QJsonObject{{"strategy", "rules"},
                 {"settings", QJsonObject{{"domainStrategy", "IPIfNonMatch"},
                                          {"rules", getRules()}}}}}};
  return v2RayConfig;
}

QJsonArray Configurator::getPrettyDnsServers(QString dnsString) {
  QJsonArray dnsServers;
  QStringList _dnsServers = dnsString.split(',');
  for (QString ds : _dnsServers) {
    dnsServers.append(ds.trimmed());
  }
  return dnsServers;
}

QJsonArray Configurator::getServers() {
  QJsonObject appConfig = getAppConfig();
  QJsonArray servers    = appConfig.contains("servers")
                         ? appConfig["servers"].toArray()
                         : QJsonArray();
  return servers;
}

QJsonObject Configurator::getServer(QString serverName) {
  QJsonArray servers = getServers();
  QJsonObject server;
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject _server = (*itr).toObject();
    if (_server.contains("serverName") &&
        _server["serverName"].toString() == serverName) {
      server = (*itr).toObject();
      break;
    }
  }
  return server;
}

int Configurator::addServer(QJsonObject serverConfig) {
  QJsonArray servers = getServers();
  servers.append(serverConfig);
  setAppConfig(QJsonObject{{"servers", servers}});
  return 1;
}

int Configurator::editServer(QString serverName, QJsonObject serverConfig) {
  QJsonArray servers = getServers();
  bool isEdited      = false;
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    if (server.contains("serverName") &&
        server["serverName"].toString() == serverName) {
      qDebug() << serverName << server;
      *itr     = serverConfig;
      isEdited = true;
    }
  }
  qDebug() << servers;
  setAppConfig(QJsonObject{{"servers", servers}});
  return isEdited;
}

int Configurator::removeServer(QString serverName) {
  QJsonArray servers = getServers();
  int serverIndex    = -1;
  for (int i = 0; i < servers.size(); ++i) {
    QJsonObject server = servers[i].toObject();
    if (server.contains("serverName") &&
        server["serverName"].toString() == serverName) {
      serverIndex = i;
      break;
    }
  }
  servers.removeAt(serverIndex);
  setAppConfig(QJsonObject{{"servers", servers}});
  return serverIndex != -1;
}

QJsonArray Configurator::getConnectedServers() {
  QJsonObject appConfig = getAppConfig();
  int muxValue          = appConfig["mux"].toString().toInt();

  QJsonArray servers = getServers();
  QJsonArray connectedServers;
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    QString serverName =
      server.contains("serverName") ? server["serverName"].toString() : "";

    if (connectedServerNames.contains(serverName)) {
      server.remove("autoConnect");
      if (muxValue > 0 && server["protocol"].toString() == "vmess") {
        server["mux"] = QJsonObject{{"enabled", true}, {"mux", muxValue}};
      }
      connectedServers.append(server);
    }
  }
  connectedServers.append(
    QJsonObject{{"tag", "direct"}, {"protocol", "freedom"}});
  return connectedServers;
}

QStringList Configurator::getConnectedServerNames() {
  return connectedServerNames;
}

void Configurator::setServerConnection(QString serverName, bool connected) {
  int serverIndex = connectedServerNames.indexOf(serverName);
  if (connected && serverIndex == -1) {
    connectedServerNames.append(serverName);
  } else if (!connected && serverIndex != -1) {
    connectedServerNames.removeAt(serverIndex);
  }
}

QJsonArray Configurator::getRules() {
  // Ref: https://v2ray.com/chapter_02/03_routing.html
  QJsonArray rules;
  rules.append(
    QJsonObject{{"outboundTag", "direct"},
                {"type", "field"},
                {"ip", QJsonArray{"geoip:cn", "geoip:private"}},
                {"domain", QJsonArray{"geosite:cn", "geosite:speedtest"}}});
  return rules;
}
