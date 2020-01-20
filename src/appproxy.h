#ifndef APPPROXY_H
#define APPPROXY_H

#include <QObject>
#include <QString>

#include "v2raycore.h"

class AppProxy: public QObject {
    Q_OBJECT
public:
    AppProxy();

signals:
    void appVersionReady(QString appVersion);
    void v2RayCoreVersionReady(QString v2RayCoreVersion);
    void v2RayCoreStatusReady(QString v2RayCoreStatus);
    void v2RayRunningStatusChanging(bool isChanged);

public slots:
    void getAppVersion();
    void getV2RayCoreVersion();
    void getV2RayCoreStatus();
    void setV2RayCoreRunning(bool expectedRunning);

private:
    V2RayCore v2ray;
};

#endif // APPPROXY_H
