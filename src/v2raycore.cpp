#include "v2raycore.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "configurator.h"
#include "constants.h"
#include "httprequest.h"
#include "zipfile.h"

V2RayCore::V2RayCore() {
  QString v2RayInstallFolderPath = Configurator::getV2RayInstallDirPath();
#if defined(Q_OS_WIN)
  v2RayExecFilePath    = QDir(v2RayInstallFolderPath).filePath("v2ray.exe");
  v2RayCtlExecFilePath = QDir(v2RayInstallFolderPath).filePath("v2ctl.exe");
#elif defined(Q_OS_LINUX) or defined(Q_OS_MAC)
  v2RayExecFilePath       = QDir(v2RayInstallFolderPath).filePath("v2ray");
  v2RayCtlExecFilePath    = QDir(v2RayInstallFolderPath).filePath("v2ctl");
#endif
  QDir v2RayInstallFolder(v2RayInstallFolderPath);
  // Create the install folder if not exists
  if (!v2RayInstallFolder.exists()) {
    v2RayInstallFolder.mkpath(".");
  }
  // Initialize QProcess
  v2rayProcess = new QProcess();
}

V2RayCore& V2RayCore::getInstance() {
  static V2RayCore v2RayCoreInstance;
  return v2RayCoreInstance;
}

V2RayCore::~V2RayCore() {
  v2rayProcess->close();
  delete v2rayProcess;
}

bool V2RayCore::start() {
  if (!isInstalled()) {
    if (!install()) {
      return false;
    }
  }
  // Get latest configuration for V2Ray Core
  Configurator& configurator(Configurator::getInstance());
  QJsonObject v2RayConfig = configurator.getV2RayConfig();
  QString configFilePath  = Configurator::getV2RayConfigFilePath();
  QFile configFile(Configurator::getV2RayConfigFilePath());
  configFile.open(QFile::WriteOnly);
  configFile.write(QJsonDocument(v2RayConfig).toJson());
  configFile.flush();

  // Start V2Ray Core
  QStringList arguments;
  arguments << "-config" << configFilePath;
  v2rayProcess->start(v2RayExecFilePath, arguments);
  v2rayProcess->waitForFinished(500);
  int exitCode = v2rayProcess->exitCode();
  if (exitCode != 0) {
    qCritical() << "Failed to start V2Ray Core.";
    qCritical() << v2rayProcess->readAllStandardOutput();
  }
  return exitCode == 0;
}

bool V2RayCore::stop() {
  v2rayProcess->terminate();
  v2rayProcess->waitForFinished();
  return v2rayProcess->state() == QProcess::NotRunning;
}

bool V2RayCore::restart() {
  stop();
  start();
  return isRunning();
}

bool V2RayCore::isRunning() {
  return v2rayProcess->state() == QProcess::Running;
}

bool V2RayCore::isInstalled() { return QFile(v2RayExecFilePath).exists(); }

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
  QByteArray assetsBytes = HttpRequest::get(assetsUrl);
  QString v2rayZipFilePath =
    QDir(QDir::currentPath())
      .filePath(QString("v2ray-core-%1.zip").arg(latestVersion));
  QFile v2RayZipFile(v2rayZipFilePath);
  if (!v2RayZipFile.open(QIODevice::WriteOnly)) {
    return false;
  }
  v2RayZipFile.write(assetsBytes);
  v2RayZipFile.close();

  // Unzip the file and make v2ray executable
  QString v2RayInstallFolderPath = Configurator::getV2RayInstallDirPath();
  ZipFile::unzipFile(v2rayZipFilePath, v2RayInstallFolderPath);
  QFile(v2RayExecFilePath)
    .setPermissions(QFileDevice::ReadUser | QFileDevice::WriteOwner |
                    QFileDevice::ExeUser);
  QFile(v2RayCtlExecFilePath)
    .setPermissions(QFileDevice::ReadUser | QFileDevice::WriteOwner |
                    QFileDevice::ExeUser);

  // Save current V2Ray version to config.json
  Configurator& configurator(Configurator::getInstance());
  QJsonObject v2RayVersionConfig{{"v2rayCoreVersion", latestVersion}};
  configurator.setAppConfig(v2RayVersionConfig);

  return true;
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
