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
    QJsonObject getConfig();
    void setConfig(QJsonObject config);
    QJsonArray getServers();
    QJsonArray getRules();

private:
    static QJsonObject DEFAULT_APP_CONFIG;
};

#endif // CONFIGURATOR_H
