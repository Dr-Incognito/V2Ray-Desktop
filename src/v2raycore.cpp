#include "v2raycore.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "constants.h"
#include "httprequest.h"
#include "zipfile.h"

V2RayCore::V2RayCore() {
  v2RayInstallFolderPath = QDir(QDir::currentPath()).filePath("v2ray-core");
  QDir v2RayInstallFolder(v2RayInstallFolderPath);
  // Create the install folder if not exists
  if (!v2RayInstallFolder.exists()) {
      v2RayInstallFolder.mkpath(".");
  }
}

bool V2RayCore::start() {
  if (!isInstalled()) {
    if (!install()) {
      return false;
    }
  }
}

bool V2RayCore::stop() {}

bool V2RayCore::restart() {}

bool V2RayCore::isInstalled() {
#if defined(Q_OS_WIN)
  QString v2RayExecPath = QDir(v2RayInstallFolderPath).filePath("v2ray.exe");
#elif defined(Q_OS_LINUX)
  QString v2RayExecPath = QDir(v2RayInstallFolderPath).filePath("v2ray");
#elif defined(Q_OS_MAC)
  QString v2RayExecPath = QDir(v2RayInstallFolderPath).filePath("v2ray");
#endif
}

bool V2RayCore::install() {
  QString latestVersion = getLatestVersion();
#if defined(Q_OS_WIN)
  QString operatingSystem = "windows-64";
#elif defined(Q_OS_LINUX)
  QString operatingSystem = "linux-64";
#elif defined(Q_OS_MAC)
  QString operatingSystem = "macos";
#endif
  // Download the zip file from GitHub
  QString assetsUrl =
    QString(V2RAY_ASSETS_URL).arg(latestVersion, operatingSystem);
  qDebug() << "Before downloading ...";
  QByteArray assetsBytes = HttpRequest::get(assetsUrl);
  qDebug() << "After downloading ...";
  QString v2rayZipFilePath =
    QDir(QDir::currentPath())
      .filePath(QString("v2ray-core-%1.zip").arg(latestVersion));
  QFile v2RayZipFile(v2rayZipFilePath);
  if (!v2RayZipFile.open(QIODevice::WriteOnly)) {
    return false;
  }
  v2RayZipFile.write(assetsBytes);
  v2RayZipFile.close();

  // Unzip the file
  ZipFile::unzipFile(v2rayZipFilePath, v2RayInstallFolderPath);

  // Save current V2Ray version to config.json
}

bool V2RayCore::upgrade() {
  // Rename old version

  // Install new version
  bool isUpgraded = install();

  // Remove old version if install successfully
  if (!isUpgraded) {
    return false;
  }
}

QString V2RayCore::getLatestVersion() {
  QByteArray releaseJsonStr = HttpRequest::get(V2RAY_RELEASES_URL);
  QJsonObject latestRelease;

  if (!releaseJsonStr.size()) {
    qDebug() << "releaseJsonStr.size()" << releaseJsonStr.size();
    return DEFAULT_V2RAY_CORE_VERSION;
  }

  QJsonDocument releaseJsonDoc = QJsonDocument::fromJson(releaseJsonStr);
  QJsonArray releases          = releaseJsonDoc.array();
  for (int i = 0; i < releases.size(); ++i) {
    QJsonObject release = releases[i].toObject();
    if (release.contains("prerelease") && !release["prerelease"].toBool()) {
      latestRelease = release;
      break;
    }
  }
  return latestRelease.empty() ? DEFAULT_V2RAY_CORE_VERSION
                               : latestRelease["name"].toString();
}
