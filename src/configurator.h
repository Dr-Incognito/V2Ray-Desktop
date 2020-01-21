#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

class Configurator: public QObject
{
    Q_OBJECT
public:
    Configurator();
    QJsonObject getAppConfig();
    QJsonObject getV2RayConfig();
    void setAppConfig(QJsonObject config);
    QJsonArray getServers();
    int addServer(QJsonObject serverConfig);
    int editServer(QString serverName, QJsonObject serverConfig);
    int removeServer(QString serverName);
    QJsonArray getRules();

private:
    static QJsonObject DEFAULT_APP_CONFIG;
    QJsonArray getPrettyDnsServers(QString dnsString);
    QJsonArray getAutoConnectServers();
};

#endif // CONFIGURATOR_H
