#ifndef PACSERVER_H
#define PACSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "configurator.h"
#include "networkproxy.h"

class PacServer : public QObject {
  Q_OBJECT
 public:
  explicit PacServer(QObject* parent = nullptr);
  ~PacServer();
  bool start(QString addr, int port);
  bool stop();
  bool isRunning();

 public slots:
  void onNewConnection();

 private:
  Configurator& configurator;
  QTcpServer* server;
  QString getLocalProxy();
  QString getPacRules();
};

#endif  // PACSERVER_H
