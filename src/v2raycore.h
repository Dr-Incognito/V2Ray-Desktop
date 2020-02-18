#ifndef V2RAYCORE_H
#define V2RAYCORE_H

#include <QNetworkProxy>
#include <QProcess>
#include <QString>

class V2RayCore : public QObject {
  Q_OBJECT
 public:
  static V2RayCore& getInstance();
  V2RayCore(V2RayCore const&) = delete;
  void operator=(V2RayCore const&) = delete;
  ~V2RayCore();
  QString getVersion();
  bool isRunning();
  bool isUpgradable();
  bool start();
  bool stop();
  bool restart();
  bool upgrade(const QString& version, const QNetworkProxy* proxy = nullptr);

 private:
  V2RayCore();
  bool isInstalled();
  QProcess* v2rayProcess;
  QString v2RayExecFilePath;
  QString v2RayCtlExecFilePath;
};

#endif  // V2RAYCORE_H
