#ifndef UTILITY_H
#define UTILITY_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

class Utility : public QObject {
  Q_OBJECT
 public:
  explicit Utility(QObject *parent = nullptr) : QObject(parent) {}
  static QString getNumericConfigError(const QJsonObject &serverConfig,
                                       const QString &key,
                                       const QString &name,
                                       int lowerBound,
                                       int upperBound);
  static QString getStringConfigError(
    const QJsonObject &serverConfig,
    const QString &key,
    const QString &name,
    const QList<std::function<bool(const QString &)>> &checkpoints = {},
    const QString &notPassedMsg = tr("The value of '%1' seems invalid."));
  static bool isIpAddrValid(const QString &ipAddr);
  static bool isIpAddrListValid(const QString &ipAddrList);
  static bool isDomainNameValid(const QString &domainName);
  static bool isUrlValid(const QString &url);
  static bool isFileExists(const QString &filePath);
  static bool isServerNameNotUsed(const QString &serverName);
};

#endif  // UTILITY_H
