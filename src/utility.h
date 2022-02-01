#ifndef UTILITY_H
#define UTILITY_H

#include <QJsonObject>
#include <QList>
#include <QNetworkProxy>
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
    bool allowEmpty                                                = false,
    const QString &notPassedMsg = tr("The value of '%1' seems invalid."));
  static bool isIpAddrValid(const QString &ipAddr);
  static bool isIpAddrListValid(const QString &ipAddrList);
  static bool isDomainNameValid(const QString &domainName);
  static bool isUrlValid(const QString &url);
  static bool isFileExists(const QString &filePath);
  static bool isServerNameNotUsed(const QString &serverName);
  static QStringList getAlpn(const QString &alpn);
  static bool isAlpnValid(const QString &alpn);
  static QString formatV2RayLog(const QString &log);
  static QString getLatestRelease(const QString &releaseUrl,
                                  const QNetworkProxy *proxy);
  static bool isVersionNewer(const QString &currentVersion,
                             const QString &version);
  static QList<int> getVersion(QString version);
};

#endif  // UTILITY_H
