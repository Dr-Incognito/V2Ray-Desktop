#include "configurator.h"

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

Configurator::Configurator() {}

QJsonObject Configurator::getConfig() {
  QFile appCfgFile(APP_CFG_FILE_PATH);
  if (!appCfgFile.exists() ||
      !appCfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return DEFAULT_APP_CONFIG;
  }
  QJsonDocument configDoc = QJsonDocument::fromJson(appCfgFile.readAll());
  QJsonObject config      = configDoc.object();
  return config;
}

void Configurator::setConfig(QJsonObject config) {
  // Load current configuration
  QJsonObject _config = getConfig();
  // Overwrite new values to current configuration
  for (auto itr = config.begin(); itr != config.end(); ++itr) {
    QString configName   = itr.key();
    QVariant configValue = itr.value().toVariant();
    switch (configValue.type()) {
      case QVariant::Bool: _config[configName] = configValue.toBool(); break;
      case QVariant::String:
        _config[configName] = configValue.toString();
        break;
      default:
        qWarning() << "Ignore unknown config item [Name=" << configName
                   << ", Type=" << configValue.type() << "]";
        break;
    }
  }
  // Save config to file
  QFile configFile(APP_CFG_FILE_PATH);
  configFile.open(QFile::WriteOnly);
  configFile.write(QJsonDocument(_config).toJson());
}

QJsonArray Configurator::getServers() {}

QJsonArray Configurator::getRules() {}
