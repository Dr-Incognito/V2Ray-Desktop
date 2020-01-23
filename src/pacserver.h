#ifndef PACSERVER_H
#define PACSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

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
  QTcpServer* server;
};

#endif  // PACSERVER_H
