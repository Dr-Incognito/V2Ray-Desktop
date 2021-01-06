#include "configurator.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QLocale>
#include <QProcessEnvironment>
#include <QStandardPaths>

#include "constants.h"

QJsonObject Configurator::DEFAULT_APP_CONFIG = {
  {"autoStart", DEFAULT_AUTO_START},
  {"hideWindow", DEFAULT_HIDE_WINDOW},
  {"enableSysProxy", AUTO_ENABLE_SYS_PROXY},
  {"defaultSysProxyProtocol", DEFAULT_SYS_PROXY_PROTOCOL},
  {"language", DEFAULT_LANGUAGE},
  {"serverIp", DEFAULT_SERVER_IP},
  {"httpPort", DEFAULT_HTTP_PORT},
  {"socksPort", DEFAULT_SOCKS_PORT},
  {"dns", DEFAULT_DNS_SERVER},
  {"proxyMode", DEFAULT_PROXY_MODE},
  {"gfwListUrl", DEFAULT_GFW_LIST_URL},
  {"gfwListLastUpdated", "Never"}};

QString Configurator::getDefaultLanguage() {
  const static QMap<QLocale::Language, QString> LANGUAGES{
    {QLocale::Chinese, "zh-CN"}};
  QLocale::Language systemLocale = QLocale::system().language();
  return LANGUAGES.contains(systemLocale) ? LANGUAGES[systemLocale] : "en-US";
}

Configurator::Configurator() { connectedServerNames = getAutoConnectServers(); }

QStringList Configurator::getAutoConnectServers() {
  QStringList autoConnectedServers;
  QJsonArray servers = getServers();
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    if (server["autoConnect"].toBool() && server.contains("name")) {
      autoConnectedServers.append(server["name"].toString());
    }
  }
  return autoConnectedServers;
}

Configurator &Configurator::getInstance() {
  static Configurator configuratorInstance;
  return configuratorInstance;
}

QDir Configurator::getAppConfigDir() {
  QDir appConfigDir =
    QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
      .filePath(QCoreApplication::applicationName());
  if (!appConfigDir.exists()) {
    appConfigDir.mkpath(".");
  }
  return appConfigDir;
}

QDir Configurator::getAppTempDir() {
  QDir tempDir =
    QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
      .filePath(QCoreApplication::applicationName());
  if (!tempDir.exists()) {
    tempDir.mkpath(".");
  }
  return tempDir;
}

QString Configurator::getV2RayInstallDirPath() {
  if (!V2RAY_USE_LOCAL_INSTALL) {
    return V2RAY_SYS_INSTALL_DIR;
  }
  return QDir(QCoreApplication::applicationDirPath())
    .filePath(V2RAY_LOCAL_INSTALL_DIR);
}

QString Configurator::getLocaleDirPath() {
  return QDir(QCoreApplication::applicationDirPath()).filePath(LOCALE_DIR);
}

QString Configurator::getAppFilePath() {
  if (QProcessEnvironment::systemEnvironment().contains("APPIMAGE")) {
    return QProcessEnvironment::systemEnvironment().value("APPIMAGE");
  }
  return QGuiApplication::applicationFilePath();
}

QString Configurator::getAppWorkingDirPath() {
  if (QProcessEnvironment::systemEnvironment().contains("OWD")) {
    return QProcessEnvironment::systemEnvironment().value("OWD");
  }
  return QCoreApplication::applicationDirPath();
}

QString Configurator::getAppLogFilePath() {
  return getAppConfigDir().filePath(APP_LOG_FILE_NAME);
}

QString Configurator::getAppConfigFilePath() {
  return getAppConfigDir().filePath(APP_CFG_FILE_NAME);
}

QString Configurator::getV2RayLogFilePath() {
  return getAppConfigDir().filePath(V2RAY_CORE_LOG_FILE_NAME);
}

QString Configurator::getV2RayConfigFilePath() {
  return getAppConfigDir().filePath(V2RAY_CORE_CFG_FILE_NAME);
}

QString Configurator::getGfwListFilePath() {
  return getAppConfigDir().filePath(GFW_LIST_FILE_NAME);
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
    appCfgFile.close();
  }
  // Replace old proxy mode with a newer value
  QString proxyMode = config["proxyMode"].toString();
  if (proxyMode == "pac" || proxyMode == "global" || proxyMode == "manual") {
    config["proxyMode"] = "rule";
  }
  // Replace old GFW List URL with a newer value
  QString _glu =
    "https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt";
  QString gfwListUrl = config["gfwListUrl"].toString();
  if (gfwListUrl == _glu) {
    config["gfwListUrl"]         = DEFAULT_GFW_LIST_URL;
    config["gfwListLastUpdated"] = "Never";
  }
  return config;
}

QString Configurator::getLanguage() {
  QJsonObject appConfig = getAppConfig();
  return appConfig.contains("language") ? appConfig["language"].toString()
                                        : getDefaultLanguage();
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
      case QVariant::Double:
      case QVariant::Int:
      case QVariant::LongLong: _config[configName] = configValue.toInt(); break;
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
  QJsonArray connectedServerNames;
  for (QString csn : getConnectedServerNames()) {
    connectedServerNames.append(csn);
  }

  QJsonObject v2RayConfig{
    {"port", appConfig["httpPort"].toInt()},
    {"socks-port", appConfig["socksPort"].toInt()},
    {"allow-lan", true},
    {"bind-address", appConfig["serverIp"].toString()},
    {"mode", "rule"},
    {"log-level", "info"},
    {"dns", QJsonObject{{"enable", false},
                        {"listen", "0.0.0.0:53"},
                        {"nameserver",
                         getPrettyDnsServers(appConfig["dns"].toString())}}},
    {"proxies", getConnectedServers()},
    {"proxy-groups",
     QJsonArray{{QJsonObject{{"name", "PROXY"},
                             {"type", "load-balance"},
                             {"proxies", connectedServerNames},
                             {"url", "http://www.gstatic.com/generate_204"},
                             {"interval", 300}}}}},
    {"rules", getRules()}};

  return v2RayConfig;
}

QJsonArray Configurator::getPrettyDnsServers(QString dnsString) {
  QJsonArray dnsServers;
  QStringList _dnsServers = dnsString.split(';');
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
    if (_server.contains("name") && _server["name"].toString() == serverName) {
      server = (*itr).toObject();
      break;
    }
  }
  return server;
}

QStringList Configurator::getSubscriptionUrls() {
  QStringList subscriptionUrls;
  QJsonArray servers = getServers();
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    if (server.contains("subscription")) {
      QString subscriptionUrl = server["subscription"].toString();
      if (!subscriptionUrls.contains(subscriptionUrl)) {
        subscriptionUrls.append(subscriptionUrl);
      }
    }
  }
  return subscriptionUrls;
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
    if (server.contains("name") && server["name"].toString() == serverName) {
      serverConfig["subscription"] = server.contains("subscription")
                                       ? server["subscription"].toString()
                                       : "";
      *itr                         = serverConfig;
      isEdited                     = true;
    }
  }
  // Update the server in connected servers
  if (connectedServerNames.contains(serverName)) {
    connectedServerNames.removeAt(connectedServerNames.indexOf(serverName));
    connectedServerNames.append(serverConfig["name"].toString());
  }
  // Update servers in the config
  setAppConfig(QJsonObject{{"servers", servers}});
  return isEdited;
}

int Configurator::removeServer(QString serverName) {
  QJsonArray servers = getServers();
  int serverIndex    = -1;
  for (int i = 0; i < servers.size(); ++i) {
    QJsonObject server = servers[i].toObject();
    if (server.contains("name") && server["name"].toString() == serverName) {
      serverIndex = i;
      break;
    }
  }
  servers.removeAt(serverIndex);
  // Remove the server from connected servers
  if (connectedServerNames.contains(serverName)) {
    connectedServerNames.removeAt(connectedServerNames.indexOf(serverName));
  }
  // Update servers in the config
  setAppConfig(QJsonObject{{"servers", servers}});
  return serverIndex != -1;
}

QMap<QString, QJsonObject> Configurator::removeSubscriptionServers(
  QString subscriptionUrl) {
  QJsonArray servers = getServers();
  QMap<QString, QJsonObject> removedServers;
  QJsonArray remainingServers;

  for (int i = 0; i < servers.size(); ++i) {
    QJsonObject server = servers[i].toObject();
    QString serverName =
      server.contains("name") ? server["name"].toString() : "";

    if (server.contains("subscription") &&
        server["subscription"].toString() == subscriptionUrl) {
      removedServers[serverName] = server;
      // Remove the server from connected servers
      if (connectedServerNames.contains(serverName)) {
        connectedServerNames.removeAt(connectedServerNames.indexOf(serverName));
      }
      continue;
    }
    remainingServers.append(server);
  }
  // Update servers in the config
  setAppConfig(QJsonObject{{"servers", remainingServers}});
  return removedServers;
}

QJsonArray Configurator::getConnectedServers() {
  QJsonObject appConfig = getAppConfig();

  QJsonArray servers = getServers();
  QJsonArray connectedServers;
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    QString serverName =
      server.contains("name") ? server["name"].toString() : "";

    if (connectedServerNames.contains(serverName)) {
      server.remove("autoConnect");
      server.remove("subscription");
      connectedServers.append(server);
    }
  }
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
  QJsonObject appConfig = getAppConfig();
  QString proxyMode     = appConfig["proxyMode"].toString();
  QString defaultAct    = "MATCH, PROXY";

  QJsonArray rules;
  if (proxyMode == "Direct") {
    defaultAct = "MATCH, DIRECT";
  } else if (proxyMode == "Global") {
    defaultAct = "MATCH, PROXY";
  } else {
    QJsonArray userRules    = getGfwListRules();
    QJsonArray gfwListRules = getGfwListRules();
    if (userRules.size() + gfwListRules.size() == 0) {
      defaultAct = "MATCH, PROXY";
    }
    rules.append("IP-CIDR, 127.0.0.0/8, DIRECT");
    rules.append("IP-CIDR, 10.0.0.0/8, DIRECT");
    rules.append("IP-CIDR, 172.16.0.0/12, DIRECT");
    rules.append("IP-CIDR, 192.168.0.0/16, DIRECT");
    for (auto itr = userRules.begin(); itr != userRules.end(); ++itr) {
      rules.append(*itr);
    }
    for (auto itr = gfwListRules.begin(); itr != gfwListRules.end(); ++itr) {
      rules.append(*itr);
    }
    rules.append("GEOIP, CN, DIRECT");
  }
  rules.append(defaultAct);
  return rules;
}

QJsonArray Configurator::getGfwListRules() {
  return getRules(getGfwListFilePath());
}

QJsonArray Configurator::getUserRules() {
  QJsonArray rules;
  // TODO
  return rules;
}

QJsonArray Configurator::getRules(const QString &fileName) {
  static const QStringList SUPPORTED_MATCHES{
    "DOMAIN-SUFFIX", "DOMAIN",   "DOMAIN-KEYWORD", "IP-CIDR",
    "SRC-IP-CIDR",   "DST-PORT", "SRC-PORT"};
  static const QStringList SUPPORTED_ACTS{"PROXY", "DIRECT", "REJECT"};
  QJsonArray rules;
  QFile ruleFile(fileName);

  if (ruleFile.exists() &&
      ruleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    while (!ruleFile.atEnd()) {
      QString line = ruleFile.readLine().trimmed();
      if (!line.startsWith('-')) {
        continue;
      }

      QStringList rule = line.mid(1).split(',');
      if (rule.size() != 3) {
        qWarning() << QString("Ignore rule: %1 in %2").arg(line, fileName);
        continue;
      }
      QString match  = rule.at(0).trimmed().toUpper();
      QString value  = rule.at(1).trimmed().toLower();
      QString action = rule.at(2).trimmed().toUpper();

      if (!SUPPORTED_MATCHES.contains(match)) {
        qWarning() << QString("Unsupported match: %1 for rule: %2 in %3")
                        .arg(match, line, fileName);
        continue;
      }
      if (!SUPPORTED_ACTS.contains(action)) {
        qWarning() << QString("Unsupported match: %1 for rule: %2 in %3")
                        .arg(match, line, fileName);
        continue;
      }
      rules.append(QString("%1, %2, %3").arg(match, value, action));
    }
  }
  return rules;
}
