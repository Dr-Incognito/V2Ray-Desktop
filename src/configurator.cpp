#include "configurator.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>

#include "constants.h"

QJsonObject Configurator::DEFAULT_APP_CONFIG = {
  {"autoStart", true},
  {"hideWindow", false},
  {"autoUpdate", true},
  {"enableUdp", false},
  {"serverProtocol", "HTTP/HTTPS"},
  {"ipAddress", "127.0.0.1"},
  {"port", "1080"},
  {"pacPort", "1085"},
  {"mux", -1},
  {"dns", "8.8.8.8,8.8.4.4"}
};

Configurator::Configurator() {
}

QJsonObject Configurator::getConfig() {
  QFile appCfgFile(APP_CFG_FILE_PATH);
  if (!appCfgFile.exists() || !appCfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return DEFAULT_APP_CONFIG;
  }
  QJsonDocument configDoc = QJsonDocument::fromJson(appCfgFile.readAll());
  QJsonObject config      = configDoc.object();
  return config;
}

QJsonArray Configurator::getServers() {

}

QJsonArray Configurator::getRules() {

}
