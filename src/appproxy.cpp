#include "appproxy.h"

#include <QDebug>

#include "constants.h"

AppProxy::AppProxy() {}

void AppProxy::getAppVersion() {
  QString appVersion = QString("%1.%2.%3")
          .arg(QString::number(APP_VERSION_MAJOR), QString::number(APP_VERSION_MINOR), QString::number(APP_VERSION_PATCH));
  emit appVersionReady(appVersion);
}

void AppProxy::getV2RayCoreVersion() {

}

void AppProxy::getV2RayCoreStatus() {
  bool isInstalled = v2ray.isInstalled();
  bool isRunning = v2ray.isRunning();
  QString v2rayStatus = isInstalled ? (isRunning ? "Running" : "Stopped") : "Not Installed";
  emit v2RayCoreStatusReady(v2rayStatus);
}

void AppProxy::setV2RayCoreRunning(bool expectedRunning) {
  if (expectedRunning) {
    emit v2RayRunningStatusChanging(v2ray.start());
  } else {
    emit v2RayRunningStatusChanging(v2ray.stop());
  }

}
