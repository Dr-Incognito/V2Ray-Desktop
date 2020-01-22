#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QString>

class HttpRequest : public QObject {
  Q_OBJECT
 public:
  HttpRequest();
  static QByteArray get(QString url);
};

#endif  // HTTPREQUEST_H
