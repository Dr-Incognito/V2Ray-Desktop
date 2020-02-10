#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

class RunGuard : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(RunGuard);

 public:
  explicit RunGuard(const QString& key, QObject* parent = nullptr);
  ~RunGuard();
  bool tryToRun();

 private:
  bool isAnotherRunning();
  void release();
  static QString hash(const QString& key, const QString& salt);

  const QString KEY;
  const QString MEM_LOCK_KEY;
  const QString SHARED_MEM_KEY;

  QSharedMemory sharedMemory;
  QSystemSemaphore memoryLock;
};

#endif  // RUNGUARD_H
