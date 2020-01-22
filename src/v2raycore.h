#ifndef V2RAYWRAPPER_H
#define V2RAYWRAPPER_H

#include <QProcess>
#include <QString>

class V2RayCore : public QObject {
  Q_OBJECT
 public:
  static V2RayCore& getInstance();
  V2RayCore(V2RayCore const&) = delete;
  void operator=(V2RayCore const&) = delete;
  ~V2RayCore();
  bool start();
  bool stop();
  bool restart();
  bool isRunning();
  bool isInstalled();

 private:
  V2RayCore();
  bool install();
  bool upgrade();
  QString getLatestVersion();
  QProcess* v2rayProcess;
  QString v2RayExecFilePath;
  QString v2RayCtlExecFilePath;
};

#endif  // V2RAYWRAPPER_H
