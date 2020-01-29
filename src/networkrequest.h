#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <QNetworkProxy>
#include <QObject>
#include <QString>

class NetworkRequest : public QObject {
  Q_OBJECT
 public:
  NetworkRequest();
  static QByteArray getNetworkResponse(QString url,
                           QNetworkProxy* proxy = nullptr,
                           int timeout          = 0);
  static int getLatency(QString host, int port);
};

#endif  // NETWORKREQUEST_H
