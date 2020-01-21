#include "appproxy.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QSysInfo>

#include "constants.h"

AppProxy::AppProxy() : v2ray(V2RayCore::getInstance()) {}

QString AppProxy::getAppVersion() {
  QString appVersion = QString("v%1.%2.%3")
                         .arg(QString::number(APP_VERSION_MAJOR),
                              QString::number(APP_VERSION_MINOR),
                              QString::number(APP_VERSION_PATCH));
  emit appVersionReady(appVersion);
  return appVersion;
}

QString AppProxy::getV2RayCoreVersion() {
  QJsonObject appConfig    = getAppConfig();
  QString v2RayCoreVersion = appConfig["v2rayCoreVersion"].toString();
  emit v2RayCoreVersionReady(v2RayCoreVersion);
  return v2RayCoreVersion;
}

QString AppProxy::getOperatingSystem() {
  QString operatingSystem = QSysInfo::prettyProductName();
  emit operatingSystemReady(operatingSystem);
  return operatingSystem;
}

QString AppProxy::getV2RayCoreStatus() {
  bool isInstalled = v2ray.isInstalled();
  bool isRunning   = v2ray.isRunning();
  QString v2rayStatus =
    isInstalled ? (isRunning ? "Running" : "Stopped") : "Not Installed";
  emit v2RayCoreStatusReady(v2rayStatus);
  return v2rayStatus;
}

QJsonObject AppProxy::getAppConfig() {
  QJsonObject appConfig = configurator.getConfig();
  emit appConfigReady(QJsonDocument(appConfig).toJson());
  return appConfig;
}

void AppProxy::saveAppConfig(QString appConfig) {
  QJsonDocument jsonDoc = QJsonDocument::fromJson(appConfig.toUtf8());
  configurator.setConfig(jsonDoc.object());
  emit appConfigChanged();
}

QString AppProxy::getLogs() {
  QFile appLogFile(APP_LOG_FILE_PATH);
  QStringList logs;
  if (appLogFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QList<QByteArray> _logList = appLogFile.readAll().split('\n');
    for (auto itr = _logList.end() - 2; itr >= _logList.begin(); -- itr) {
      logs.append(*itr);
    }
  }
  QString _logs = logs.join('\n');
  emit logsReady(_logs);
  return _logs;
}

void AppProxy::clearLogs() {
  QFile appLogFile(APP_LOG_FILE_PATH);
  if (appLogFile.exists()) {
      appLogFile.remove();
  }
}

bool AppProxy::setV2RayCoreRunning(bool expectedRunning) {
  bool isSuccessful = false;
  if (expectedRunning) {
    isSuccessful = v2ray.start();
    qInfo() << QString("Start V2Ray Core ... %1")
                 .arg(isSuccessful ? "success" : "failed");
    emit v2RayRunningStatusChanging(isSuccessful);
  } else {
    isSuccessful = v2ray.stop();
    qInfo() << QString("Stop V2Ray Core ... %1")
                 .arg(isSuccessful ? "success" : "failed");
    emit v2RayRunningStatusChanging(isSuccessful);
  }
  return isSuccessful;
}
