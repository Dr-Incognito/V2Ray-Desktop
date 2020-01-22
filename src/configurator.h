#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

class Configurator: public QObject
{
    Q_OBJECT
public:
    static Configurator& getInstance();
    Configurator(Configurator const&)       = delete;
    void operator=(Configurator const&)  = delete;
    QJsonObject getAppConfig();
    QJsonObject getV2RayConfig();
    void setAppConfig(QJsonObject config);
    QJsonArray getServers();
    QJsonObject getServer(QString serverName);
    int addServer(QJsonObject serverConfig);
    int editServer(QString serverName, QJsonObject serverConfig);
    int removeServer(QString serverName);
    QJsonArray getConnectedServers();
    QStringList getConnectedServerNames();
    void setServerConnection(QString serverName, bool connected);
    QJsonArray getRules();

private:
    Configurator();
    static QJsonObject DEFAULT_APP_CONFIG;
    QStringList connectedServerNames;
    QJsonArray getPrettyDnsServers(QString dnsString);
    QStringList getAutoConnectServers();
};

#endif // CONFIGURATOR_H
