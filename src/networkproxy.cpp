#include "networkproxy.h"

#include <QDebug>
#include <QMap>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>

NetworkProxy NetworkProxyHelper::getSystemProxy() {
#if defined(Q_OS_WIN)
  return getSystemProxyWindows();
#elif defined(Q_OS_MAC)
  return getSystemProxyMacOs();
#elif defined(Q_OS_LINUX)
  QString desktopEnv =
    QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP");
  if (desktopEnv.contains("GNOME") || desktopEnv.contains("Unity")) {
    return getSystemProxyLinuxGnome();
  } else if (desktopEnv.contains("KDE")) {
    return getSystemProxyLinuxKde();
  } else {
    return NetworkProxy();
  }
#else
  return NetworkProxy();
#endif
}

void NetworkProxyHelper::setSystemProxy(const NetworkProxy& proxy) {
#if defined(Q_OS_WIN)
  return setSystemProxyWindows(proxy);
#elif defined(Q_OS_MAC)
  return setSystemProxyMacOs(proxy);
#elif defined(Q_OS_LINUX)
  QString desktopEnv =
    QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP");
  if (desktopEnv.contains("GNOME") || desktopEnv.contains("Unity")) {
    return setSystemProxyLinuxGnome(proxy);
  } else if (desktopEnv.contains("KDE")) {
    return setSystemProxyLinuxKde(proxy);
  }
#endif
}

void NetworkProxyHelper::resetSystemProxy() {
#if defined(Q_OS_WIN)
  return resetSystemProxyWindows();
#elif defined(Q_OS_MAC)
  return resetSystemProxyMacOs();
#elif defined(Q_OS_LINUX)
  QString desktopEnv =
    QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP");
  if (desktopEnv.contains("GNOME") || desktopEnv.contains("Unity")) {
    return resetSystemProxyLinuxGnome();
  } else if (desktopEnv.contains("KDE")) {
    return resetSystemProxyLinuxKde();
  }
#endif
}

#if defined(Q_OS_WIN)
NetworkProxy NetworkProxyHelper::getSystemProxyWindows() {
  NetworkProxy proxy;
  QSettings internetSettings(
    "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet "
    "Settings",
    QSettings::NativeFormat);
  if (internetSettings.contains("AutoConfigURL")) {
    proxy.setMode(NetworkProxyMode::PAC_MODE);
    proxy.setHost(internetSettings.value("AutoConfigURL").toString());
    proxy.setPort(0);
  } else if (internetSettings.value("ProxyEnable").toInt() == 1 &&
             internetSettings.contains("ProxyServer")) {
    QString proxyServer = internetSettings.value("ProxyServer").toString();
    int colonIndex      = proxyServer.indexOf(':');
    proxy.setProtocol("http");
    proxy.setMode(NetworkProxyMode::GLOBAL_MODE);
    proxy.setHost(proxyServer.left(colonIndex));
    proxy.setPort(proxyServer.mid(colonIndex + 1).toInt());
  }
  return proxy;
}

void NetworkProxyHelper::setSystemProxyWindows(const NetworkProxy& proxy) {
  QSettings internetSettings(
    "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet "
    "Settings",
    QSettings::NativeFormat);
  if (proxy.getMode() == NetworkProxyMode::GLOBAL_MODE) {
    internetSettings.setValue("ProxyEnable", 1);
    internetSettings.setValue(
      "ProxyServer",
      QString("%1:%2").arg(proxy.getHost(), QString::number(proxy.getPort())));
  }
}

void NetworkProxyHelper::resetSystemProxyWindows() {
  QSettings internetSettings(
    "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet "
    "Settings",
    QSettings::NativeFormat);
  internetSettings.setValue("ProxyEnable", 0);
  internetSettings.remove("AutoConfigURL");
  internetSettings.remove("ProxyServer");
}
#elif defined(Q_OS_MAC)
const QStringList NetworkProxyHelper::NETWORK_INTERFACES = {"Wi-Fi",
                                                            "Enternet"};

NetworkProxy NetworkProxyHelper::getSystemProxyMacOs() {
  NetworkProxy proxy;
  QString stdOutput;
  for (QString ni : NETWORK_INTERFACES) {
    QProcess p;
    p.start("networksetup", QStringList{"-getautoproxyurl", ni});
    p.waitForFinished();
    stdOutput = p.readAllStandardOutput();
    if (!stdOutput.startsWith("** Error:")) {
      QStringList sl = stdOutput.split('\n');
      for (QString s : sl) {
        if (s.startsWith("Enabled: Yes")) {
          proxy.setMode(NetworkProxyMode::PAC_MODE);
        } else if (s.startsWith("URL: ")) {
          proxy.setHost(s.mid(5));
          proxy.setPort(0);
        }
      }
    }
    p.start("networksetup", QStringList{"-getwebproxy", ni});
    p.waitForFinished();
    stdOutput = p.readAllStandardOutput();
    if (!stdOutput.startsWith("** Error:")) {
      QStringList sl = stdOutput.split('\n');
      for (QString s : sl) {
        if (s.startsWith("Enabled: Yes")) {
          proxy.setMode(NetworkProxyMode::GLOBAL_MODE);
          proxy.setProtocol("http");
        } else if (proxy.getProtocol() == "http" && s.startsWith("Server: ")) {
          proxy.setHost(s.mid(8));
        } else if (proxy.getProtocol() == "http" && s.startsWith("Port: ")) {
          proxy.setPort(s.mid(6).toInt());
        }
      }
    }
    p.start("networksetup", QStringList{"-getsocksfirewallproxy", ni});
    p.waitForFinished();
    stdOutput = p.readAllStandardOutput();
    if (!stdOutput.startsWith("** Error:")) {
      QStringList sl = stdOutput.split('\n');
      for (QString s : sl) {
        if (s.startsWith("Enabled: Yes")) {
          proxy.setMode(NetworkProxyMode::GLOBAL_MODE);
          proxy.setProtocol("socks");
        } else if (proxy.getProtocol() == "socks" && s.startsWith("Server: ")) {
          proxy.setHost(s.mid(8));
        } else if (proxy.getProtocol() == "socks" && s.startsWith("Port: ")) {
          proxy.setPort(s.mid(6).toInt());
        }
      }
    }
  }
  return proxy;
}

void NetworkProxyHelper::setSystemProxyMacOs(const NetworkProxy& proxy) {
  QString protocol = proxy.getProtocol();
  static const QMap<QString, QString> SETTING_KEYS = {
    {"http", "web"}, {"socks", "socksfirewall"}};

  if (proxy.getMode() == NetworkProxyMode::GLOBAL_MODE) {
    QString settingKey = SETTING_KEYS.value(protocol);
    QProcess p;
    for (QString ni : NETWORK_INTERFACES) {
      p.start("networksetup",
              QStringList{QString("-set%1proxy").arg(settingKey), ni,
                          proxy.getHost(), QString::number(proxy.getPort())});
      p.waitForFinished();
      p.start(
        "networksetup",
        QStringList{QString("-set%1proxystate").arg(settingKey), ni, "on"});
      p.waitForFinished();
    }
  }
}

void NetworkProxyHelper::resetSystemProxyMacOs() {
  for (QString ni : NETWORK_INTERFACES) {
    QProcess p;
    p.start("networksetup", QStringList{"-setautoproxystate", ni, "off"});
    p.waitForFinished();
    p.start("networksetup", QStringList{"-setwebproxystate", ni, "off"});
    p.waitForFinished();
    p.start("networksetup",
            QStringList{"-setsocksfirewallproxystate", ni, "off"});
    p.waitForFinished();
  }
}
#elif defined(Q_OS_LINUX)
NetworkProxy NetworkProxyHelper::getSystemProxyLinuxGnome() {
  QProcess p;
  NetworkProxy proxy;
  p.start("gsettings",
          QStringList{"list-recursively", "org.gnome.system.proxy"});
  p.waitForFinished();
  QList<QByteArray> settings = p.readAllStandardOutput().split('\n');
  for (QByteArray s : settings) {
    if (s.startsWith("org.gnome.system.proxy mode")) {
      int qIndex = s.indexOf('\'');
      QString proxyMode = s.mid(qIndex + 1, s.lastIndexOf('\'') - qIndex - 1);
      if (proxyMode == "none") {
        proxy.setMode(NetworkProxyMode::DIRECT_MODE);
      } else if (proxyMode == "auto") {
        proxy.setMode(NetworkProxyMode::PAC_MODE);
      } else if (proxyMode == "manual") {
        proxy.setMode(NetworkProxyMode::GLOBAL_MODE);
      }
    }
    if (proxy.getMode() == NetworkProxyMode::PAC_MODE) {
      if (s.startsWith("org.gnome.system.proxy autoconfig-url")) {
        int qIndex = s.indexOf('\'');
        proxy.setHost(s.mid(qIndex + 1, s.lastIndexOf('\'') - qIndex - 1));
        proxy.setPort(0);
      }
    } else if (proxy.getMode() == NetworkProxyMode::GLOBAL_MODE) {
      if (s.startsWith("org.gnome.system.proxy.http port")) {
        int port = s.mid(33).toInt();
        if (port) {
          proxy.setPort(port);
        }
      } else if (s.startsWith("org.gnome.system.proxy.http host")) {
        int qIndex = s.indexOf('\'');
        QString host = s.mid(qIndex + 1, s.lastIndexOf('\'') - qIndex - 1);
        if (host.size()) {
          proxy.setHost(host);
          proxy.setProtocol("http");
        }
      } else if (s.startsWith("org.gnome.system.proxy.socks port")) {
        int port = s.mid(33).toInt();
        if (port) {
          proxy.setPort(port);
        }
      } else if (s.startsWith("org.gnome.system.proxy.socks host")) {
        int qIndex = s.indexOf('\'');
        QString host = s.mid(qIndex + 1, s.lastIndexOf('\'') - qIndex - 1);
        if (host.size()) {
          proxy.setHost(host);
          proxy.setProtocol("socks");
        }
      }
    }
  }
  return proxy;
}

void NetworkProxyHelper::setSystemProxyLinuxGnome(const NetworkProxy& proxy) {
  if (proxy.getMode() == NetworkProxyMode::GLOBAL_MODE) {
    QProcess p;
    p.start("gsettings",
            QStringList{"set", "org.gnome.system.proxy", "mode", "manual"});
    p.waitForFinished();
    p.start(
      "gsettings",
      QStringList{"set",
                  QString("org.gnome.system.proxy.%1").arg(proxy.getProtocol()),
                  "host", proxy.getHost()});
    p.waitForFinished();
    p.start(
      "gsettings",
      QStringList{"set",
                  QString("org.gnome.system.proxy.%1").arg(proxy.getProtocol()),
                  "port", QString::number(proxy.getPort())});
    p.waitForFinished();
  }
}

void NetworkProxyHelper::resetSystemProxyLinuxGnome() {
  QProcess p;
  p.start("gsettings",
          QStringList{"set", "org.gnome.system.proxy", "mode", "none"});
  p.waitForFinished();
  p.start("gsettings",
          QStringList{"set", "org.gnome.system.proxy", "ignore-hosts",
                      "['localhost', '127.0.0.0/8', '::1', '10.0.0.0/8', "
                      "'172.16.0.0/12', '192.168.0.0/16']"});
  p.waitForFinished();
  p.start("gsettings", QStringList{"set", "org.gnome.system.proxy.http",
                                   "enabled", "false"});
  p.waitForFinished();
  p.start("gsettings",
          QStringList{"set", "org.gnome.system.proxy.http", "host", ""});
  p.waitForFinished();
  p.start("gsettings",
          QStringList{"set", "org.gnome.system.proxy.http", "port", "0"});
  p.waitForFinished();
  p.start("gsettings",
          QStringList{"set", "org.gnome.system.proxy.socks", "host", ""});
  p.waitForFinished();
  p.start("gsettings",
          QStringList{"set", "org.gnome.system.proxy.socks", "port", "0"});
  p.waitForFinished();
}

NetworkProxy NetworkProxyHelper::getSystemProxyLinuxKde() {
  QProcess p;
  NetworkProxy proxy;
  p.start("kreadconfig5", QStringList{"--file", "kioslaverc", "--group",
                                      "Proxy Settings", "--key", "ProxyType"});
  p.waitForFinished();
  int proxyMode = p.readAllStandardOutput().trimmed().toInt();
  if (proxyMode == 1 || proxyMode == 4) {
    // Manual; Use System Settings
    proxy.setMode(NetworkProxyMode::GLOBAL_MODE);
  } else if (proxyMode == 2) {
    proxy.setMode(NetworkProxyMode::PAC_MODE);
  }
  if (proxy.getMode() == NetworkProxyMode::GLOBAL_MODE) {
    p.start("kreadconfig5",
            QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                        "--key", "httpProxy"});
    p.waitForFinished();
    QString httpProxy = p.readAllStandardOutput().trimmed();
    if (httpProxy.size() > 0) {
      QStringList _proxy = httpProxy.split(' ');
      proxy.setProtocol("http");
      proxy.setHost(_proxy.at(0));
      proxy.setPort(_proxy.at(1).toInt());
    }
    p.start("kreadconfig5",
            QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                        "--key", "socksProxy"});
    p.waitForFinished();
    QString socksProxy = p.readAllStandardOutput().trimmed();
    if (socksProxy.size() > 0) {
      QStringList _proxy = socksProxy.split(' ');
      proxy.setProtocol("socks");
      proxy.setHost(_proxy.at(0));
      proxy.setPort(_proxy.at(1).toInt());
    }
  } else if (proxy.getMode() == NetworkProxyMode::PAC_MODE) {
    p.start("kreadconfig5",
            QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                        "--key", "Proxy Config Script"});
    p.waitForFinished();
    proxy.setHost(p.readAllStandardOutput());
  }
  return proxy;
}

void NetworkProxyHelper::setSystemProxyLinuxKde(const NetworkProxy& proxy) {
  QProcess p;
  if (proxy.getMode() == NetworkProxyMode::GLOBAL_MODE) {
    p.start("kwriteconfig5",
            QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                        "--key", "ProxyType", "1"});
    p.waitForFinished();
    p.start("kwriteconfig5",
            QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                        "--key", QString("%1Proxy").arg(proxy.getProtocol()),
                        QString("%1 %2").arg(
                          proxy.getHost(), QString::number(proxy.getPort()))});
    p.waitForFinished();
  }
}

void NetworkProxyHelper::resetSystemProxyLinuxKde() {
  QProcess p;
  p.start("kwriteconfig5",
          QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                      "--key", "ProxyType", "0"});
  p.waitForFinished();
  p.start("kwriteconfig5",
          QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                      "--key", "Proxy Config Script", ""});
  p.waitForFinished();
  p.start("kwriteconfig5",
          QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                      "--key", "httpProxy", ""});
  p.waitForFinished();
  p.start("kwriteconfig5",
          QStringList{"--file", "kioslaverc", "--group", "Proxy Settings",
                      "--key", "socksProxy", ""});
  p.waitForFinished();
}
#endif
