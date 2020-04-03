#ifndef NETWORKPROXY_H
#define NETWORKPROXY_H

#include <QObject>

enum class NetworkProxyMode { PAC_MODE, GLOBAL_MODE, DIRECT_MODE };

struct NetworkProxy {
 public:
  NetworkProxy() { this->mode = NetworkProxyMode::DIRECT_MODE; }

  NetworkProxy(QString protocol,
               QString host,
               int port,
               NetworkProxyMode mode) {
    this->protocol = protocol;
    this->host     = host;
    this->port     = port;
    this->mode     = mode;
  }

  QString getProtocol() const { return this->protocol; }

  void setProtocol(QString protocol) { this->protocol = protocol; }

  QString getHost() const { return this->host; }

  void setHost(QString host) { this->host = host; }

  int getPort() const { return this->port; }

  void setPort(int port) { this->port = port; }

  NetworkProxyMode getMode() const { return this->mode; }

  void setMode(NetworkProxyMode mode) { this->mode = mode; }

  inline QString toString() {
    if (mode == NetworkProxyMode::DIRECT_MODE) {
      return "Disabled";
    } else if (mode == NetworkProxyMode::PAC_MODE) {
      // The host contains the URL to the PAC file.
      // For example: http://127.0.0.1:1085/proxy.pac
      return host;
    } else {
      return QString("%1://%2:%3").arg(protocol, host, QString::number(port));
    }
  }

  bool operator==(const NetworkProxy &other) {
    return this->mode == other.mode;
    if (mode == NetworkProxyMode::DIRECT_MODE) {
      return other.mode == NetworkProxyMode::DIRECT_MODE;
    } else if (mode == NetworkProxyMode::PAC_MODE) {
      return host == other.host;
    } else {
      return protocol == other.protocol && host == other.host &&
             port == other.port;
    }
  }

  // bool operator!=(const NetworkProxy& other) { return !(*this == other); }
 private:
  NetworkProxyMode mode;
  QString protocol;
  QString host;
  int port;
};

class NetworkProxyHelper : public QObject {
  Q_OBJECT
 public:
  static NetworkProxy getSystemProxy();
  static void setSystemProxy(const NetworkProxy &proxy);
  static void resetSystemProxy();

 private:
#if defined(Q_OS_WIN)
  static NetworkProxy getSystemProxyWindows();
  static void setSystemProxyWindows(const NetworkProxy &proxy);
  static void resetSystemProxyWindows();
#elif defined(Q_OS_MAC)
  static const QStringList NETWORK_INTERFACES;
  static NetworkProxy getSystemProxyMacOs();
  static void setSystemProxyMacOs(const NetworkProxy &proxy);
  static void resetSystemProxyMacOs();
#elif defined(Q_OS_LINUX)
  static NetworkProxy getSystemProxyLinuxGnome();
  static NetworkProxy getSystemProxyLinuxKde();
  static void setSystemProxyLinuxGnome(const NetworkProxy &proxy);
  static void setSystemProxyLinuxKde(const NetworkProxy &proxy);
  static void resetSystemProxyLinuxGnome();
  static void resetSystemProxyLinuxKde();
#endif
};

#endif  // NETWORKPROXY_H
