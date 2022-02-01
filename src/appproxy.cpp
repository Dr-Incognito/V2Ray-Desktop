#include "appproxy.h"

#include <QByteArray>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QNetworkProxy>
#include <QPixmap>
#include <QQmlContext>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QScreen>
#include <QSettings>
#include <QSysInfo>

#include "constants.h"
#include "networkproxy.h"
#include "networkrequest.h"
#include "qrcodehelper.h"
#include "serverconfighelper.h"
#include "utility.h"

AppProxy::AppProxy(QObject* parent)
  : QObject(parent),
    v2ray(V2RayCore::getInstance()),
    configurator(Configurator::getInstance()) {
  // Setup Worker
  worker->moveToThread(&workerThread);
  connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);

  // Setup Worker -> getServerLatency
  connect(this, &AppProxy::getServerLatencyStarted, worker,
          &AppProxyWorker::getServerLatency);
  connect(worker, &AppProxyWorker::serverLatencyReady, this,
          &AppProxy::returnServerLatency);

  // Setup Worker -> getGfwList
  connect(this, &AppProxy::getGfwListStarted, worker,
          &AppProxyWorker::getGfwList);
  connect(worker, &AppProxyWorker::gfwListReady, this,
          &AppProxy::returnGfwList);

  // Setup Worker -> getNetworkStatus
  connect(this, &AppProxy::getNetworkStatusStarted, worker,
          &AppProxyWorker::getUrlAccessibility);
  connect(worker, &AppProxyWorker::urlAccessibilityReady, this,
          &AppProxy::returnNetworkAccessiblity);

  // Setup Worker -> getSubscriptionServers
  connect(this, &AppProxy::getSubscriptionServersStarted, worker,
          &AppProxyWorker::getSubscriptionServers);
  connect(worker, &AppProxyWorker::subscriptionServersReady, this,
          &AppProxy::addSubscriptionServers);

  // Setup Worker -> getLogs
  connect(this, &AppProxy::getLogsStarted, worker, &AppProxyWorker::getLogs);
  connect(worker, &AppProxyWorker::logsReady, this, &AppProxy::returnLogs);

  // Setup Worker -> getLatestRelease
  connect(this, &AppProxy::getLatestReleaseStarted, worker,
          &AppProxyWorker::getLatestRelease);
  connect(worker, &AppProxyWorker::latestReleaseReady, this,
          &AppProxy::returnLatestRelease);

  workerThread.start();
}

AppProxy::~AppProxy() {
  NetworkProxyHelper::resetSystemProxy();
  workerThread.quit();
  workerThread.wait();
}

QString AppProxy::getAppVersion() {
  QString appVersion = QString("v%1.%2.%3")
                         .arg(QString::number(APP_VERSION_MAJOR),
                              QString::number(APP_VERSION_MINOR),
                              QString::number(APP_VERSION_PATCH));
  emit appVersionReady(appVersion);
  return appVersion;
}

void AppProxy::getV2RayCoreVersion() {
  QString v2rayVersion = v2ray.getVersion();
  QRegularExpression regx("^[0-9]");
  if (regx.match(v2rayVersion).hasMatch()) {
    v2rayVersion = QString("v%1").arg(v2rayVersion);
  }
  emit v2RayCoreVersionReady(v2rayVersion);
}

void AppProxy::getOperatingSystem() {
  QString operatingSystem = QSysInfo::prettyProductName();
  emit operatingSystemReady(operatingSystem);
}

void AppProxy::getV2RayCoreStatus() {
  emit v2RayCoreStatusReady(v2ray.isRunning());
}

void AppProxy::setV2RayCoreRunning(bool expectedRunning) {
  bool isSuccessful = false;
  if (expectedRunning) {
    isSuccessful = v2ray.start();
    qInfo()
      << QString("Start Clash ... %1").arg(isSuccessful ? "success" : "failed");
  } else {
    isSuccessful = v2ray.stop();
    qInfo()
      << QString("Stop Clash ... %1").arg(isSuccessful ? "success" : "failed");
  }
  if (isSuccessful) {
    emit v2RayCoreStatusReady(expectedRunning);
  } else {
    emit v2RayCoreStatusReady(!expectedRunning);
  }
}

void AppProxy::getNetworkStatus() {
  qRegisterMetaType<QMap<QString, bool>>("QMap");
  qRegisterMetaType<QNetworkProxy>("QNetworkProxy");
  emit getNetworkStatusStarted({{"google.com", true}, {"baidu.com", false}},
                               getQProxy());
}

QNetworkProxy AppProxy::getQProxy() {
  QStringList connectedServerNames = configurator.getConnectedServerNames();
  if (connectedServerNames.size() == 0 || !v2ray.isRunning()) {
    return QNetworkProxy::NoProxy;
  }

  QJsonObject appConfig              = configurator.getAppConfig();
  QNetworkProxy::ProxyType proxyType = QNetworkProxy::Socks5Proxy;
  int socksPort                      = appConfig["socksPort"].toInt();
  QNetworkProxy proxy;
  proxy.setType(proxyType);
  proxy.setHostName("127.0.0.1");
  proxy.setPort(socksPort);
  return proxy;
}

void AppProxy::returnNetworkAccessiblity(QMap<QString, bool> accessible) {
  bool isGoogleAccessible =
         accessible.contains("google.com") ? accessible["google.com"] : false,
       isBaiduAccessible =
         accessible.contains("baidu.com") ? accessible["baidu.com"] : false;

  emit networkStatusReady(
    QJsonDocument(QJsonObject{
                    {"isGoogleAccessible", isGoogleAccessible},
                    {"isBaiduAccessible", isBaiduAccessible},
                  })
      .toJson());
}

void AppProxy::getAppConfig() {
  QJsonObject appConfig = configurator.getAppConfig();
  emit appConfigReady(QJsonDocument(appConfig).toJson());
}

void AppProxy::setAppConfig(QString configString) {
  QJsonDocument configDoc = QJsonDocument::fromJson(configString.toUtf8());
  QJsonObject appConfig   = configDoc.object();
  // Check if app config contains errors
  QStringList appConfigErrors = getAppConfigErrors(appConfig);
  if (appConfigErrors.size() > 0) {
    emit appConfigError(appConfigErrors.join('\n'));
    return;
  }
  // Set auto start and update UI language
  setAutoStart(appConfig["autoStart"].toBool());
  retranslate(appConfig["language"].toString());
  // Save app config
  appConfig["httpPort"]  = appConfig["httpPort"].toString().toInt();
  appConfig["socksPort"] = appConfig["socksPort"].toString().toInt();
  configurator.setAppConfig(appConfig);
  qInfo() << "Application config updated. Restarting V2Ray ...";
  // Restart V2Ray Core
  v2ray.restart();
  // Notify that the app config has changed
  emit appConfigChanged();
}

QStringList AppProxy::getAppConfigErrors(const QJsonObject& appConfig) {
  QStringList errors;
  errors.append(
    Utility::getStringConfigError(appConfig, "language", tr("Language")));
  errors.append(Utility::getStringConfigError(
    appConfig, "serverIp", tr("Listening IP Address"),
    {
      std::bind(&Utility::isIpAddrValid, std::placeholders::_1),
    }));
  errors.append(Utility::getStringConfigError(
    appConfig, "dns", tr("DNS Server"),
    {
      std::bind(&Utility::isIpAddrListValid, std::placeholders::_1),
    }));
  errors.append(Utility::getNumericConfigError(appConfig, "httpPort",
                                               tr("HTTP Port"), 1, 65535));
  errors.append(Utility::getNumericConfigError(appConfig, "socksPort",
                                               tr("SOCKS Port"), 1, 65535));
  if (appConfig["httpPort"].toString() == appConfig["socksPort"].toString()) {
    errors.append(tr("'HTTP Port' and 'SOCKS Port' can not be the same."));
  }
  errors.append(Utility::getStringConfigError(
    appConfig, "gfwListUrl", tr("GFW List URL"),
    {std::bind(&Utility::isUrlValid, std::placeholders::_1)}));

  // Remove empty error messages generated by getNumericConfigError() and
  // getStringConfigError()
  errors.removeAll("");
  return errors;
}

bool AppProxy::retranslate(QString language) {
  if (language.isEmpty()) {
    Configurator& configurator(Configurator::getInstance());
    language = configurator.getLanguage();
  }
  QCoreApplication* app = QGuiApplication::instance();
  app->removeTranslator(&translator);
  bool isTrLoaded = translator.load(
    QString("%1/%2.qm").arg(Configurator::getLocaleDirPath(), language));

  app->installTranslator(&translator);
  QQmlEngine::contextForObject(this)->engine()->retranslate();
  return isTrLoaded;
}

void AppProxy::setAutoStart(bool autoStart) {
  const QString APP_NAME = "V2Ray Desktop";
  const QString APP_PATH =
    QDir::toNativeSeparators(Configurator::getAppFilePath());
#if defined(Q_OS_WIN)
  QSettings settings(
    "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
    QSettings::NativeFormat);
#elif defined(Q_OS_LINUX)
  QFile srcFile(":/misc/tpl-linux-autostart.desktop");
  QFile dstFile(QString("%1/.config/autostart/v2ray-dekstop.desktop")
                  .arg(QDir::homePath()));
#elif defined(Q_OS_MAC)
  QFile srcFile(":/misc/tpl-macos-autostart.plist");
  QFile dstFile(
    QString("%1/Library/LaunchAgents/com.v2ray.desktop.launcher.plist")
      .arg(QDir::homePath()));
#endif

#if defined(Q_OS_WIN)
  if (autoStart) {
    settings.setValue(APP_NAME, APP_PATH);
  } else {
    settings.remove(APP_NAME);
  }
#elif defined(Q_OS_LINUX) or defined(Q_OS_MAC)
  if (autoStart) {
    QString fileContent;
    if (srcFile.exists() &&
        srcFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      fileContent = srcFile.readAll();
      srcFile.close();
    }
    QFileInfo dstFileInfo(dstFile);
    if (!dstFileInfo.dir().exists()) {
      dstFileInfo.dir().mkpath(".");
    }
    if (dstFile.open(QIODevice::WriteOnly)) {
      dstFile.write(fileContent.arg(APP_PATH).toUtf8());
      dstFile.close();
    }
  } else {
    if (dstFile.exists()) {
      dstFile.remove();
    }
  }
#endif
}

void AppProxy::getLogs() {
  emit getLogsStarted(Configurator::getAppLogFilePath(),
                      Configurator::getV2RayLogFilePath());
}

void AppProxy::returnLogs(QString logs) { emit logsReady(logs); }

void AppProxy::clearLogs() {
  QFile appLogFile(Configurator::getAppLogFilePath());
  QFile v2RayLogFile(Configurator::getV2RayLogFilePath());
  if (appLogFile.exists()) {
    appLogFile.resize(0);
  }
  if (v2RayLogFile.exists()) {
    v2RayLogFile.resize(0);
  }
}

void AppProxy::getProxySettings() {
  bool isV2RayRunning   = v2ray.isRunning();
  QJsonObject appConfig = configurator.getAppConfig();
  QString systemProxy   = NetworkProxyHelper::getSystemProxy().toString();
  QJsonArray connectedServers;
  for (QString cs : configurator.getConnectedServerNames()) {
    connectedServers.append(cs);
  }

  emit proxySettingsReady(
    QJsonDocument(QJsonObject{{"isV2RayRunning", isV2RayRunning},
                              {"systemProxy", systemProxy},
                              {"proxyMode", appConfig["proxyMode"].toString()},
                              {"connectedServers", connectedServers}})
      .toJson());
}

void AppProxy::setProxyMode(QString proxyMode) {
  QJsonObject appConfig = configurator.getAppConfig();
  QString _proxyMode    = appConfig["proxyMode"].toString();
  // Automatically set system proxy according to app config
  if (!proxyMode.size()) {
    proxyMode = _proxyMode;
  }

  // Update app config
  configurator.setAppConfig({{"proxyMode", proxyMode}});
  // Detect whether the proxy mode is changed
  if (proxyMode != _proxyMode) {
    v2ray.restart();
  }
  emit proxyModeChanged(proxyMode);
}

void AppProxy::setSystemProxy(bool enableProxy, QString protocol) {
  // Allow values for 'protocol': http; socks
  QJsonObject appConfig = configurator.getAppConfig();
  if (!protocol.size()) {
    protocol = appConfig["defaultSysProxyProtocol"].toString();
  }

  // Set system proxy
  NetworkProxyHelper::resetSystemProxy();
  if (enableProxy && v2ray.isRunning()) {
    NetworkProxy proxy(protocol, "127.0.0.1",
                       appConfig[QString("%1Port").arg(protocol)].toInt(),
                       NetworkProxyMode::GLOBAL_MODE);
    NetworkProxyHelper::setSystemProxy(proxy);
  }
  configurator.setAppConfig({
    {"enableSysProxy", enableProxy},
    {"defaultSysProxyProtocol", protocol},
  });
}

void AppProxy::updateGfwList() {
  QJsonObject appConfig = configurator.getAppConfig();
  emit getGfwListStarted(appConfig["gfwListUrl"].toString(), getQProxy());
}

void AppProxy::returnGfwList(QString gfwList) {
  if (gfwList.size()) {
    QFile gfwListFile(Configurator::getGfwListFilePath());
    gfwListFile.open(QFile::WriteOnly);
    gfwListFile.write(gfwList.toUtf8());
    gfwListFile.flush();
    // Update app config
    QString updatedTime = QDateTime::currentDateTime().toString();
    configurator.setAppConfig(QJsonObject{
      {"gfwListLastUpdated", QDateTime::currentDateTime().toString()}});
    qInfo() << "GFW List updated successfully.";
    emit gfwListUpdated(updatedTime);
    // Restart V2Ray
    v2ray.restart();
  } else {
    emit gfwListUpdated(tr("Failed to update GFW List."));
  }
}

void AppProxy::getServers() {
  QJsonArray servers               = configurator.getServers();
  QStringList connectedServerNames = configurator.getConnectedServerNames();

  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    QString serverName =
      server.contains("name") ? server["name"].toString() : "";
    server["connected"] = connectedServerNames.contains(serverName);
    if (serverLatency.contains(serverName)) {
      server["latency"] = serverLatency[serverName].toInt();
    }
    *itr = server;
  }
  emit serversReady(QJsonDocument(servers).toJson());
}

void AppProxy::getServer(QString serverName, bool forDuplicate) {
  QJsonObject server = configurator.getServer(serverName);
  if (forDuplicate) {
    server.remove("name");
  }
  emit serverDInfoReady(QJsonDocument(server).toJson());
}

void AppProxy::getServerLatency(QString serverName) {
  QJsonObject _serverLatency;
  QJsonArray servers;
  if (serverName.size()) {
    servers.append(configurator.getServer(serverName));
  } else {
    servers = configurator.getServers();
  }
  qRegisterMetaType<QJsonArray>("QJsonArray");
  emit getServerLatencyStarted(servers);
}

void AppProxy::returnServerLatency(QMap<QString, QVariant> latency) {
  // Ref:
  // https://stackoverflow.com/questions/8517853/iterating-over-a-qmap-with-for
  // Note: Convert to StdMap for better performance
  for (auto l : latency.toStdMap()) {
    serverLatency[l.first] = l.second.toInt();
  }
  emit serverLatencyReady(QJsonDocument::fromVariant(latency).toJson());
}

void AppProxy::setServerConnection(QString serverName, bool connected) {
  configurator.setServerConnection(serverName, connected);
  bool isV2RayRunning = v2ray.restart();
  qInfo() << (connected ? "Connected to " : "Disconnected from ") << serverName;
  emit v2RayCoreStatusReady(isV2RayRunning);
  emit serverConnectivityChanged(serverName, connected);
}

void AppProxy::addServer(QString protocol, QString configString) {
  ServerConfigHelper::Protocol _protocol =
    ServerConfigHelper::getProtocol(protocol);
  QJsonDocument configDoc  = QJsonDocument::fromJson(configString.toUtf8());
  QJsonObject serverConfig = configDoc.object();
  // Check server config before saving
  QStringList serverConfigErrors =
    ServerConfigHelper::getServerConfigErrors(_protocol, serverConfig);
  if (serverConfigErrors.size() > 0) {
    emit serverConfigError(serverConfigErrors.join('\n'));
    return;
  }
  // Save server config
  configurator.addServer(
    ServerConfigHelper::getPrettyServerConfig(_protocol, serverConfig));
  emit serversChanged();
  qInfo() << QString("Add new %1 server [Name=%2, Addr=%3].")
               .arg(protocol, serverConfig["serverName"].toString(),
                    serverConfig["serverAddr"].toString());
}

void AppProxy::addServerUrl(QString serverUrl) {
  if (serverUrl.startsWith("vmess://") || serverUrl.startsWith("ss://") ||
      serverUrl.startsWith("ssr://") || serverUrl.startsWith("trojan://")) {
    addSubscriptionServers(serverUrl);
  } else {
    addSubscriptionUrl(serverUrl);
  }
}

void AppProxy::addSubscriptionUrl(QString subsriptionUrl) {
  QString error = Utility::getStringConfigError(
    {{"subsriptionUrl", subsriptionUrl}}, "subsriptionUrl",
    tr("Subscription URL"),
    {std::bind(&Utility::isUrlValid, std::placeholders::_1)});
  if (!error.isEmpty()) {
    emit serverConfigError(error);
    return;
  }
  updateSubscriptionServers(subsriptionUrl);
}

void AppProxy::updateSubscriptionServers(QString subsriptionUrl) {
  QStringList subscriptionUrls;
  if (subsriptionUrl.isEmpty()) {
    // Sync servers from subscription
    subscriptionUrls = configurator.getSubscriptionUrls();
  } else {
    // Add a new server subscription
    subscriptionUrls.append(subsriptionUrl);
  }
  for (QString su : subscriptionUrls) {
    emit getSubscriptionServersStarted(su, getQProxy());
  }
}

void AppProxy::addSubscriptionServers(QString subsriptionServers,
                                      QString subsriptionUrl) {
  if (!subsriptionServers.size()) {
    emit serverConfigError(tr("Failed to get subscription servers from URLs."));
    return;
  }
  // Remove servers from the subscription if exists
  QMap<QString, QJsonObject> removedServers;
  if (!subsriptionUrl.isEmpty()) {
    configurator.removeSubscriptionServers(subsriptionUrl);
  }
  // Add new servers
  int nImportedServers = 0;
  QStringList servers  = subsriptionServers.split('\n');
  for (QString server : servers) {
    ServerConfigHelper::Protocol protocol =
      ServerConfigHelper::Protocol::UNKNOWN;
    if (server.startsWith("ss://") || server.startsWith("ssr://")) {
      protocol = ServerConfigHelper::Protocol::SHADOWSOCKS;
    } else if (server.startsWith("vmess://")) {
      protocol = ServerConfigHelper::Protocol::VMESS;
    } else if (server.startsWith("trojan://")) {
      protocol = ServerConfigHelper::Protocol::TROJAN;
    }
    QJsonObject serverConfig = ServerConfigHelper::getServerConfigFromUrl(
      protocol, server, subsriptionUrl);
    QStringList serverConfigErrors =
      ServerConfigHelper::getServerConfigErrors(protocol, serverConfig);
    serverConfig =
      ServerConfigHelper::getPrettyServerConfig(protocol, serverConfig);
    if (!serverConfigErrors.empty()) {
      qWarning() << QString("Error occurred for the server URL: %1. Errors: %2")
                      .arg(server, serverConfigErrors.join(" "));
      continue;
    }
    // Recover auto connect option for the server
    QString serverName =
      serverConfig.contains("name") ? serverConfig["name"].toString() : "";
    serverConfig["autoConnect"] =
      removedServers.contains(serverName)
        ? removedServers[serverName]["autoConnect"].toBool()
        : false;
    // Save the server
    configurator.addServer(serverConfig);
    qInfo() << QString("Add a new server[Name=%1] from URI: %2")
                 .arg(serverName, server);
    ++nImportedServers;
  }
  if (nImportedServers) {
    emit serversChanged();
  } else {
    emit serverConfigError(tr("No supported servers added from the URL."));
  }
}

void AppProxy::addServerConfigFile(QString configFilePath,
                                   QString configFileType) {
  QFile configFile(configFilePath);
  if (!configFile.exists()) {
    emit serverConfigError(tr("The config file does not exist."));
    return;
  }
  configFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QJsonDocument configDoc = QJsonDocument::fromJson(configFile.readAll());
  configFile.close();

  int nAddServers                       = 0;
  ServerConfigHelper::Protocol protocol = ServerConfigHelper::Protocol::UNKNOWN;
  QList<QJsonObject> servers;
  if (configFileType == "v2ray-config") {
    protocol = ServerConfigHelper::Protocol::VMESS;
    servers =
      ServerConfigHelper::getServerConfigFromV2RayConfig(configDoc.object());
  } else if (configFileType == "shadowsocks-qt5-config") {
    protocol = ServerConfigHelper::Protocol::SHADOWSOCKS;
    servers  = ServerConfigHelper::getServerConfigFromShadowsocksQt5Config(
      configDoc.object());
  }
  for (QJsonObject server : servers) {
    QStringList serverConfigErrors =
      ServerConfigHelper::getServerConfigErrors(protocol, server);
    if (!serverConfigErrors.empty()) {
      qWarning() << QString(
                      "Error occurred for the server[Name=%1]. Errors: %2")
                      .arg(server["name"].toString(),
                           serverConfigErrors.join(" "));
      continue;
    } else {
      configurator.addServer(
        ServerConfigHelper::getPrettyServerConfig(protocol, server));
      qInfo()
        << QString(
             "Add a new server[Name=%1] from Shadowsocks-Qt5 config file.")
             .arg(server["name"].toString());
      ++nAddServers;
    }
  }
  if (nAddServers) {
    emit serversChanged();
  } else {
    emit serverConfigError(
      tr("No supported servers added from the config file."));
  }
}

void AppProxy::editServer(QString serverName,
                          QString protocol,
                          QString configString) {
  QJsonDocument configDoc = QJsonDocument::fromJson(configString.toUtf8());
  ServerConfigHelper::Protocol _protocol =
    ServerConfigHelper::getProtocol(protocol);
  QJsonObject serverConfig       = configDoc.object();
  QStringList serverConfigErrors = ServerConfigHelper::getServerConfigErrors(
    _protocol, serverConfig, &serverName);
  if (serverConfigErrors.size() > 0) {
    emit serverConfigError(serverConfigErrors.join('\n'));
    return;
  }
  serverConfig =
    ServerConfigHelper::getPrettyServerConfig(_protocol, serverConfig);

  if (configurator.editServer(serverName, serverConfig)) {
    QString newServerName = serverConfig["name"].toString();
    // Update the information of server connectivity
    QStringList connectedServerNames = configurator.getConnectedServerNames();
    serverConfig["connected"] = connectedServerNames.contains(newServerName);
    // Update the server latency even if the server name is changed
    if (serverLatency.contains(serverName)) {
      serverConfig["latency"] = serverLatency[serverName].toInt();
      if (newServerName != serverName) {
        serverLatency.insert(newServerName, serverLatency[serverName]);
        serverLatency.remove(serverName);
      }
    }
    emit serverChanged(serverName, QJsonDocument(serverConfig).toJson());
    // Restart V2Ray Core
    v2ray.restart();
  }
}

void AppProxy::removeServer(QString serverName) {
  configurator.removeServer(serverName);
  qInfo() << QString("Server [Name=%1] has been removed.").arg(serverName);
  emit serverRemoved(serverName);
  // Restart V2Ray Core
  v2ray.restart();
}

void AppProxy::removeSubscriptionServers(QString subscriptionUrl) {
  configurator.removeSubscriptionServers(subscriptionUrl);
  qInfo() << QString("Servers from Subscription %1 have been removed.")
               .arg(subscriptionUrl);
  emit serversChanged();
}

void AppProxy::scanQrCodeScreen() {
  QStringList servers;
  QList<QScreen*> screens = QGuiApplication::screens();

  for (int i = 0; i < screens.size(); ++i) {
    QRect r = screens.at(i)->geometry();
    QPixmap screenshot =
      screens.at(i)->grabWindow(0, r.x(), r.y(), r.width(), r.height());
    QString serverUrl = QrCodeHelper::decode(
      screenshot.toImage().convertToFormat(QImage::Format_Grayscale8));
    if (serverUrl.size()) {
      servers.append(serverUrl);
    }
  }
  qInfo() << QString("Add %1 servers from QR code.")
               .arg(QString::number(servers.size()));
  addSubscriptionServers(servers.join('\n'));
}

void AppProxy::copyToClipboard(QString text) {
  QClipboard* clipboard = QGuiApplication::clipboard();
  clipboard->setText(text, QClipboard::Clipboard);
}

void AppProxy::getLatestRelease(QString name) {
  if (!latestVersion.contains(name)) {
    QDateTime initTime = QDateTime::currentDateTime();
    initTime.setSecsSinceEpoch(0);
    latestVersion[name] = {
      {"currentVersion", ""},
      {"latestVersion", ""},
      {"checkTime", initTime},
    };
  }

  if (latestVersion[name]["checkTime"].toDateTime().secsTo(
        QDateTime::currentDateTime()) < RELEASE_CHECK_INTERVAL) {
    emit latestReleaseReady(name,
                            latestVersion[name]["latestVersion"].toString());
    return;
  }
  if (name == "v2ray-core") {
    latestVersion[name]["currentVersion"] = v2ray.getVersion();
    emit getLatestReleaseStarted(name, V2RAY_RELEASES_URL, getQProxy());
  } else if (name == "v2ray-desktop") {
    latestVersion[name]["currentVersion"] = getAppVersion();
    emit getLatestReleaseStarted(name, APP_RELEASES_URL, getQProxy());
  }
}

void AppProxy::returnLatestRelease(QString name, QString version) {
  if (version.isEmpty()) {
    emit latestReleaseError(name, tr("Failed to check updates"));
    return;
  }
  if (!Utility::isVersionNewer(latestVersion[name]["currentVersion"].toString(),
                               version)) {
    version = "";
  }
  latestVersion[name]["checkTime"]     = QDateTime::currentDateTime();
  latestVersion[name]["latestVersion"] = version;
  emit latestReleaseReady(name, version);
}

void AppProxy::upgradeDependency(QString name, QString version) {
  if (!V2RAY_USE_LOCAL_INSTALL) {
    emit upgradeError(name, tr("Please upgrade from the package manager"));
    return;
  } else if (name == "v2ray-core" &&
             QProcessEnvironment::systemEnvironment().contains("APPIMAGE")) {
    emit upgradeError(name,
                      tr("The V2Ray Core in AppImage is not upgradable."));
    return;
  }
  if (name == "v2ray-core") {
#if defined(Q_OS_WIN)
    QString operatingSystem = "windows-amd64";
#elif defined(Q_OS_LINUX)
    QString operatingSystem = "linux-amd64";
#elif defined(Q_OS_MAC)
    QString operatingSystem = "darwin-amd64";
#else
    QString operatingSystem = "unknown";
#endif
    // TODO
    emit upgradeStarted(
      name, V2RAY_ASSETS_URL.arg(version, operatingSystem, version),
      Configurator::getAppTempDir().filePath(name), getQProxy());
  } else if (name == "v2ray-desktop") {
#if defined(Q_OS_WIN)
    QString operatingSystem = "win64";
    QString fileExtension   = "zip";
#elif defined(Q_OS_LINUX)
    QString operatingSystem = "linux-x86_64";
    QString fileExtension   = "AppImage";
#elif defined(Q_OS_MAC)
    QString operatingSystem = "macOS";
    QString fileExtension   = "zip";
#else
    QString operatingSystem = "unknown";
    QString fileExtension   = "";
#endif
    emit upgradeStarted(
      name,
      APP_ASSETS_URL.arg(version, version, operatingSystem, fileExtension),
      Configurator::getAppTempDir().filePath(name), getQProxy());
  }
}

void AppProxy::replaceDependency(QString name,
                                 QString outputFilePath,
                                 QString errorMsg) {
  if (errorMsg.isEmpty()) {
    if (name == "v2ray-core") {
      v2ray.stop();
      if (!replaceV2RayCoreFiles(Configurator::getV2RayInstallDirPath(),
                                 outputFilePath)) {
        errorMsg = tr("Failed to replace V2Ray Core files.");
      }
      v2ray.start();
    } else if (name == "v2ray-desktop") {
      // TODO: Upgrade the app itself with an external upgrader
    }
  }
  if (!errorMsg.isEmpty()) {
    emit upgradeError(name, errorMsg);
  } else {
    emit upgradeCompleted(name);
  }
}

bool AppProxy::replaceV2RayCoreFiles(const QString& srcFolderPath,
                                     const QString& dstFolderPath) {
#if defined(Q_OS_WIN)
  QString v2RayExecFilePath    = QDir(dstFolderPath).filePath("v2ray.exe");
  QString v2RayCtlExecFilePath = QDir(dstFolderPath).filePath("v2ctl.exe");
#elif defined(Q_OS_LINUX) or defined(Q_OS_MAC)
  QString v2RayExecFilePath    = QDir(dstFolderPath).filePath("v2ray");
  QString v2RayCtlExecFilePath = QDir(dstFolderPath).filePath("v2ctl");
#endif
  QFile(v2RayExecFilePath)
    .setPermissions(QFileDevice::ReadUser | QFileDevice::WriteOwner |
                    QFileDevice::ExeUser);
  QFile(v2RayCtlExecFilePath)
    .setPermissions(QFileDevice::ReadUser | QFileDevice::WriteOwner |
                    QFileDevice::ExeUser);

  QDir srcFolder(srcFolderPath);
  if (srcFolder.exists() && !srcFolder.removeRecursively()) {
    // TODO: fallback if errors occurred
    return false;
  }
  return QDir().rename(dstFolderPath, srcFolderPath);
}
