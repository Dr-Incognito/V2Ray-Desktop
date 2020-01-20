#ifndef V2RAYWRAPPER_H
#define V2RAYWRAPPER_H

#include <QString>
#include <QProcess>

class V2RayCore: public QObject {
    Q_OBJECT
public:
    V2RayCore();
    bool start();
    bool stop();
    bool restart();
    bool isRunning();
    bool isInstalled();

private:
    bool install();
    bool upgrade();
    QString getLatestVersion();
    QProcess* v2rayProcess;
    QString v2RayInstallFolderPath;
    QString v2RayExecFilePath;
    QString v2RayCtlExecFilePath;
};

#endif // V2RAYWRAPPER_H
