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
  if (desktopEnv == "GNOME") {
    return getSystemProxyLinuxGnome();
  } else if (desktopEnv == "KDE") {
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
  if (desktopEnv == "GNOME") {
    return setSystemProxyLinuxGnome(proxy);
  } else if (desktopEnv == "KDE") {
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
  if (desktopEnv == "GNOME") {
    return resetSystemProxyLinuxGnome();
  } else if (desktopEnv == "KDE") {
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
  } else if (internetSettings.value("ProxyEnable").toInt() == 1 &&
             internetSettings.contains("ProxyServer")) {
    QString proxyServer = internetSettings.value("ProxyServer").toString();
    int colonIndex      = proxyServer.indexOf(':');
    proxy.setProtocol("socks");
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
  QString stdOutput, socksProxyHost;
  int socksProxyPort;
  for (QString ni : NETWORK_INTERFACES) {
    QProcess p;
    p.start("networksetup", QStringList() << "-getautoproxyurl" << ni);
    p.waitForFinished();
    stdOutput = p.readAllStandardOutput();
    if (!stdOutput.startsWith("** Error:")) {
      QStringList sl = stdOutput.split('\n');
      for (QString s : sl) {
        if (s.startsWith("Enabled: Yes")) {
          proxy.setMode(NetworkProxyMode::PAC_MODE);
        } else if (s.startsWith("URL: ")) {
          proxy.setHost(s.mid(5));
        }
      }
    }
    p.start("networksetup", QStringList() << "-getwebproxy" << ni);
    p.waitForFinished();
    stdOutput = p.readAllStandardOutput();
    if (!stdOutput.startsWith("** Error:")) {
      QStringList sl = stdOutput.split('\n');
      for (QString s : sl) {
        if (s.startsWith("Enabled: Yes")) {
          proxy.setMode(NetworkProxyMode::GLOBAL_MODE);
          proxy.setProtocol("http");
        } else if (s.startsWith("Server: ")) {
          socksProxyHost = s.mid(8);
        } else if (s.startsWith("Port: ")) {
          socksProxyPort = s.mid(6).toInt();
        }
      }
    }
    p.start("networksetup", QStringList() << "-getsocksfirewallproxy" << ni);
    p.waitForFinished();
    stdOutput = p.readAllStandardOutput();
    if (!stdOutput.startsWith("** Error:")) {
      QStringList sl = stdOutput.split('\n');
      for (QString s : sl) {
        if (s.startsWith("Enabled: Yes")) {
          proxy.setMode(NetworkProxyMode::GLOBAL_MODE);
          proxy.setProtocol("socks");
        } else if (s.startsWith("Server: ")) {
          socksProxyHost = s.mid(8);
        } else if (s.startsWith("Port: ")) {
          socksProxyPort = s.mid(6).toInt();
        }
      }
    }
  }
  return proxy;
}

void NetworkProxyHelper::setSystemProxyMacOs(const NetworkProxy& proxy) {
  if (proxy.getMode() == NetworkProxyMode::GLOBAL_MODE) {
    QProcess p;
    for (QString ni : NETWORK_INTERFACES) {
      p.start("networksetup",
              QStringList{"-setsocksfirewallproxy", proxy.getHost(),
                          QString::number(proxy.getPort())});
      p.waitForFinished();
      p.start("networksetup", QStringList{"-setsocksfirewallproxystate", "on"});
      p.waitForFinished();
    }
  }
}

void NetworkProxyHelper::resetSystemProxyMacOs() {
  for (QString ni : NETWORK_INTERFACES) {
    QProcess p;
    p.start("networksetup", QStringList()
                              << "-setautoproxystate" << ni << "off");
    p.waitForFinished();
    p.start("networksetup", QStringList()
                              << "-setwebproxystate" << ni << "off");
    p.waitForFinished();
    p.start("networksetup", QStringList()
                              << "-setsocksfirewallproxystate" << ni << "off");
    p.waitForFinished();
  }
}
#elif defined(Q_OS_LINUX)
NetworkProxy NetworkProxyHelper::getSystemProxyLinuxGnome() {
  QProcess p;
  NetworkProxy proxy;
  p.start("gsettings", QStringList() << "list-recursively"
                                     << "org.gnome.system.proxy");
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
    } else if (s.startsWith("org.gnome.system.proxy autoconfig-url")) {
      int qIndex = s.indexOf('\'');
      proxy.setHost(s.mid(qIndex + 1, s.lastIndexOf('\'') - qIndex - 1));
    } else if (s.startsWith("org.gnome.system.proxy.http port")) {
      proxy.setPort(s.mid(33).toInt());
    } else if (s.startsWith("org.gnome.system.proxy.http host")) {
      int qIndex = s.indexOf('\'');
      proxy.setHost(s.mid(qIndex + 1, s.lastIndexOf('\'') - qIndex - 1));
      proxy.setProtocol("http");
    } else if (s.startsWith("org.gnome.system.proxy.socks port")) {
      proxy.setPort(s.mid(33).toInt());
    } else if (s.startsWith("org.gnome.system.proxy.socks host")) {
      int qIndex = s.indexOf('\'');
      proxy.setHost(s.mid(qIndex + 1, s.lastIndexOf('\'') - qIndex - 1));
      proxy.setProtocol("socks");
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
    p.start("gsettings", QStringList{"set", "org.gnome.system.proxy.socks",
                                     "host", proxy.getHost()});
    p.waitForFinished();
    p.start("gsettings", QStringList{"set", "org.gnome.system.proxy.socks",
                                     "port", QString::number(proxy.getPort())});
    p.waitForFinished();
  }
}

void NetworkProxyHelper::resetSystemProxyLinuxGnome() {
  QProcess p;
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy"
                                     << "mode"
                                     << "none");
  p.waitForFinished();
  p.start("gsettings", QStringList()
                         << "set"
                         << "org.gnome.system.proxy"
                         << "ignore-hosts"
                         << "['localhost', '127.0.0.0/8', '::1', '10.0.0.0/8', "
                            "'172.16.0.0/12', '192.168.0.0/16']");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.http"
                                     << "enabled"
                                     << "false");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.http"
                                     << "host"
                                     << "");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.http"
                                     << "port"
                                     << "0");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.socks"
                                     << "host"
                                     << "");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.socks"
                                     << "port"
                                     << "0");
  p.waitForFinished();
}

NetworkProxy NetworkProxyHelper::getSystemProxyLinuxKde() {
  // TODO
  return NetworkProxy();
}

void NetworkProxyHelper::setSystemProxyLinuxKde(const NetworkProxy& proxy) {
  // TODO
}

void NetworkProxyHelper::resetSystemProxyLinuxKde() {
  // TODO
}
#endif
