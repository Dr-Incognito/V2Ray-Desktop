#include "appproxyworker.h"
#include "networkrequest.h"

#include <QDebug>

AppProxyWorker::AppProxyWorker(QObject *parent) : QObject(parent) {}

void AppProxyWorker::getServerLatency(QJsonArray servers) {
  QMap<QString, QVariant> serverLatency;
  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server = (*itr).toObject();
    QString serverName = server["serverName"].toString();
    int latency =
      NetworkRequest::getLatency(getServerAddr(server), getServerPort(server));
    serverLatency[serverName] = latency;
  }
  emit serverLatencyReady(serverLatency);
}

QString AppProxyWorker::getServerAddr(QJsonObject server) {
  QString protocol     = server["protocol"].toString();
  QString keyName      = protocol == "vmess" ? "vnext" : "servers";
  QJsonObject settings = server["settings"].toObject();
  QJsonArray servers   = settings[keyName].toArray();
  QJsonObject _server  = servers.at(0).toObject();
  return _server["address"].toString();
}

int AppProxyWorker::getServerPort(QJsonObject server) {
  QString protocol     = server["protocol"].toString();
  QString keyName      = protocol == "vmess" ? "vnext" : "servers";
  QJsonObject settings = server["settings"].toObject();
  QJsonArray servers   = settings[keyName].toArray();
  QJsonObject _server  = servers.at(0).toObject();
  return _server["port"].toInt();
}

void AppProxyWorker::getGfwList(QString gfwListUrl) {
  QByteArray gfwList =
    QByteArray::fromBase64(NetworkRequest::getNetworkResponse(gfwListUrl));
  qRegisterMetaType<QByteArray>("QByteArray");
  emit gfwListReady(gfwList);
}
