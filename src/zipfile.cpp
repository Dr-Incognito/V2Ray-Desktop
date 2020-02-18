#include "zipfile.h"

#include <QDebug>
#include <QDir>

#include "3rdparty/quazip/quazip/quazip.h"
#include "3rdparty/quazip/quazip/quazipfile.h"

bool ZipFile::unzipFile(QString zipFilePath, QString destFolder) {
  QuaZip zip(zipFilePath);
  if (!zip.open(QuaZip::mdUnzip)) {
    qWarning("testRead(): zip.open(): %d", zip.getZipError());
    return false;
  }

  zip.setFileNameCodec("IBM866");
  qWarning("%d entries\n", zip.getEntriesCount());
  qWarning("Global comment: %s\n", zip.getComment().toLocal8Bit().constData());

  QuaZipFileInfo info;
  QuaZipFile file(&zip);
  QFile out;
  QString name;
  char c;
  for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
    if (!zip.getCurrentFileInfo(&info)) {
      qWarning("testRead(): getCurrentFileInfo(): %d\n", zip.getZipError());
      return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
      qWarning("testRead(): file.open(): %d", file.getZipError());
      return false;
    }

    name = QString("%1/%2").arg(destFolder).arg(file.getActualFileName());
    if (!QDir(destFolder).exists()) {
      if (!QDir(destFolder).mkpath(".")) {
        qWarning() << QString("Failed to create folder: %1").arg(destFolder);
        return false;
      }
    }

    if (file.getZipError() != UNZ_OK) {
      qWarning("testRead(): file.getFileName(): %d", file.getZipError());
      return false;
    }
    // out.setFileName("out/" + name);
    out.setFileName(name);

    // this will fail if "name" contains subdirectories, but we don't mind that
    out.open(QIODevice::WriteOnly);
    // Slow like hell (on GNU/Linux at least), but it is not my fault.
    // Not ZIP/UNZIP package's fault either.
    // The slowest thing here is out.putChar(c).
    while (file.getChar(&c)) {
      out.putChar(c);
    }
    out.close();

    if (file.getZipError() != UNZ_OK) {
      qWarning("testRead(): file.getFileName(): %d", file.getZipError());
      return false;
    }

    if (!file.atEnd()) {
      qWarning("testRead(): read all but not EOF");
      return false;
    }
    file.close();

    if (file.getZipError() != UNZ_OK) {
      qWarning("testRead(): file.close(): %d", file.getZipError());
      return false;
    }
  }
  zip.close();

  if (zip.getZipError() != UNZ_OK) {
    qWarning("testRead(): zip.close(): %d", zip.getZipError());
    return false;
  }
  return true;
}
