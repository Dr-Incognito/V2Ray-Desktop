#ifndef V2RAYWRAPPER_H
#define V2RAYWRAPPER_H

#include <QString>
#include <QProcess>

class V2RayCore {
public:
    V2RayCore();
    bool start();
    bool stop();
    bool restart();

private:
    bool isInstalled();
    bool install();
    bool upgrade();
    QString getLatestVersion();
    QProcess v2rayProcess;
    QString v2RayInstallFolderPath;
};

#endif // V2RAYWRAPPER_H
