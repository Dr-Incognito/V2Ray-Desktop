#ifndef NETWORKPROXY_H
#define NETWORKPROXY_H

#include <QObject>

enum class NetworkProxyType { PAC_PROXY, HTTP_PROXY, SOCK_PROXY };

struct NetworkProxy {
  NetworkProxyType type;
  QString host;
  int port;
  QString url;

  inline QString toString() {
    if (type == NetworkProxyType::PAC_PROXY) {
      return url;
    } else if (type == NetworkProxyType::HTTP_PROXY) {
      return QString("http://%1:%2").arg(host, port);
    } else if (type == NetworkProxyType::SOCK_PROXY) {
      return QString("socks://%1:%2").arg(host, port);
    }
  }

  bool operator==(const NetworkProxy& other) {
    return this->type == other.type;
    if (type == NetworkProxyType::PAC_PROXY) {
      return this->url == other.url;
    } else {
      return this->host == other.host && this->port == other.port;
    }
  }

  bool operator!=(const NetworkProxy& other) { return !(*this == other); }
};

class NetworkProxyHelper : public QObject {
  Q_OBJECT
 public:
  static NetworkProxy getSystemProxy();
  static void setSystemProxy(NetworkProxy proxy);
  static void resetSystemProxy();

 private:
  static QStringList NETWORK_INTERFACES;
};

#endif  // NETWORKPROXY_H
