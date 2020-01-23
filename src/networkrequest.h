#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <QObject>
#include <QString>

class NetworkRequest : public QObject {
  Q_OBJECT
 public:
  NetworkRequest();
  static QByteArray getUrl(QString url);
  static int getLatency(QString host, int port);
};

#endif  // NETWORKREQUEST_H
