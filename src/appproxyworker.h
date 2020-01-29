#ifndef APPPROXYWORKER_H
#define APPPROXYWORKER_H

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>
#include <QVariant>

class AppProxyWorker : public QObject {
  Q_OBJECT
 public:
  explicit AppProxyWorker(QObject *parent = nullptr);

 public slots:
  void getServerLatency(QJsonArray servers);
  void getGfwList(QString gfwListUrl);

 signals:
  void serverLatencyReady(QMap<QString, QVariant> latency);
  void gfwListReady(QByteArray gfwList);

 private:
  QString getServerAddr(QJsonObject server);
  int getServerPort(QJsonObject server);
};

#endif  // APPPROXYWORKER_H
