#include "runguard.h"

#include <QCryptographicHash>

QString RunGuard::hash(const QString &key, const QString &salt) {
  QByteArray hashValue;
  hashValue.append(key.toUtf8());
  hashValue.append(salt.toUtf8());

  return QCryptographicHash::hash(hashValue, QCryptographicHash::Sha1).toHex();
}

RunGuard::RunGuard(const QString &key, QObject *parent)
  : QObject(parent),
    KEY(key),
    MEM_LOCK_KEY(hash(key, "MemLockKey")),
    SHARED_MEM_KEY(hash(key, "SharedMemoryKey")),
    sharedMemory(SHARED_MEM_KEY),
    memoryLock(MEM_LOCK_KEY, 1) {
  memoryLock.acquire();
  {
    // Fix for *nix: http://habrahabr.ru/post/173281/
    QSharedMemory fix(SHARED_MEM_KEY);
    fix.attach();
  }
  memoryLock.release();
}

RunGuard::~RunGuard() { release(); }

bool RunGuard::isAnotherRunning() {
  if (sharedMemory.isAttached()) {
    return false;
  }
  memoryLock.acquire();
  const bool isRunning = sharedMemory.attach();
  if (isRunning) {
    sharedMemory.detach();
  }
  memoryLock.release();

  return isRunning;
}

bool RunGuard::tryToRun() {
  if (isAnotherRunning()) {
    return false;
  }

  memoryLock.acquire();
  const bool result = sharedMemory.create(sizeof(quint64));
  memoryLock.release();
  if (!result) {
    release();
    return false;
  }
  return true;
}

void RunGuard::release() {
  memoryLock.acquire();
  if (sharedMemory.isAttached()) {
    sharedMemory.detach();
  }
  memoryLock.release();
}
