#include "networkproxy.h"

#include <QDebug>
#include <QMap>
#include <QProcess>

QStringList NetworkProxyHelper::NETWORK_INTERFACES{"Wi-Fi", "Enternet"};

NetworkProxy NetworkProxyHelper::getSystemProxy() {
#if defined(Q_OS_WIN)
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_MAC)
  QStringList checkpoints = {"-getautoproxyurl", "-getwebproxy",
                             "-getsocksfirewallproxy"};

  /*QProcess p;
  p.start("networksetup", QStringList() << "-getautoproxyurl" << "Wi-Fi");
  p.waitForFinished();
  qDebug() << p.readAllStandardOutput();*/
#endif
}

void NetworkProxyHelper::setSystemProxy(NetworkProxy proxy) {
#if defined(Q_OS_WIN)
#elif defined(Q_OS_LINUX)
  // Support GNOME only
  QMap<NetworkProxyType, QList<QStringList> > settingCommands{
    {NetworkProxyType::PAC_PROXY,
     {{"set", "org.gnome.system.proxy", "mode", "auto"},
      {"set", "org.gnome.system.proxy", "autoconfig-url", proxy.url}}},
    {NetworkProxyType::SOCK_PROXY,
     {{"set", "org.gnome.system.proxy", "mode", "manual"},
      {"set", "org.gnome.system.proxy.socks", "host", proxy.host},
      {"set", "org.gnome.system.proxy.socks", "port",
       QString::number(proxy.port)}}},
    {NetworkProxyType::HTTP_PROXY,
     {{"set", "org.gnome.system.proxy", "mode", "manual"},
      {"set", "org.gnome.system.proxy.http", "host", proxy.host},
      {"set", "org.gnome.system.proxy.http", "port",
       QString::number(proxy.port)}}},
  };
  QProcess p;
  QList<QStringList> commands = settingCommands[proxy.type];
  for (QStringList c : commands) {
    p.start("gsettings", c);
    p.waitForFinished();
  }
#elif defined(Q_OS_MAC)
  QMap<NetworkProxyType, QList<QStringList> > settingCommands{
    {NetworkProxyType::PAC_PROXY,
     {{"-setautoproxyurl", proxy.url}, {"-setautoproxystate", "on"}}},
    {NetworkProxyType::SOCK_PROXY,
     {{"-setsocksfirewallproxy", proxy.host, QString::number(proxy.port)},
      {"-setsocksfirewallproxystate", "on"}}},
    {NetworkProxyType::HTTP_PROXY,
     {{"-setwebproxy", proxy.host, QString::number(proxy.port)},
      {"-setwebproxystate", "on"}}},
  };
  for (QString ni : NETWORK_INTERFACES) {
    QProcess p;
    QList<QStringList> commands = settingCommands[proxy.type];
    for (QStringList c : commands) {
      c.insert(1, ni);
      p.start("networksetup", c);
      p.waitForFinished();
      qDebug() << p.readAllStandardOutput();
    }
  }
#endif
}

void NetworkProxyHelper::resetSystemProxy() {
#if defined(Q_OS_WIN)
#elif defined(Q_OS_LINUX)
  QProcess p;
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy"
                                     << "mode"
                                     << "disabled");
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
                                     << "disabled");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.http"
                                     << "host"
                                     << "");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.http"
                                     << "port"
                                     << "");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.socks"
                                     << "host"
                                     << "");
  p.waitForFinished();
  p.start("gsettings", QStringList() << "set"
                                     << "org.gnome.system.proxy.socks"
                                     << "port"
                                     << "");
  p.waitForFinished();
#elif defined(Q_OS_MAC)
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
#endif
}
