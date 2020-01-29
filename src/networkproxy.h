#ifndef NETWORKPROXY_H
#define NETWORKPROXY_H

#include <QDebug>
#include <QObject>

enum class NetworkProxyType { PAC_PROXY, HTTP_PROXY, SOCKS_PROXY, DISABLED };

struct NetworkProxy {
  NetworkProxyType type;
  QString host;
  int port;
  QString url;

  inline QString toString() {
    if (type == NetworkProxyType::PAC_PROXY) {
      return QString("[PAC Proxy] %1").arg(url);
    } else if (type == NetworkProxyType::HTTP_PROXY) {
      return QString("[HTTP Proxy] http://%1:%2")
        .arg(host, QString::number(port));
    } else if (type == NetworkProxyType::SOCKS_PROXY) {
      return QString("[SOCKS Proxy] socks://%1:%2")
        .arg(host, QString::number(port));
    } else {
      return "[Disabled]";
    }
  }

  bool operator==(const NetworkProxy& other) {
    return this->type == other.type;
    if (type == NetworkProxyType::PAC_PROXY) {
      return this->url == other.url;
    } else if (type == NetworkProxyType::HTTP_PROXY ||
               type == NetworkProxyType::SOCKS_PROXY) {
      return this->host == other.host && this->port == other.port;
    } else {
      return true;
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
