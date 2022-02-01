#include "utility.h"

#include <functional>

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QList>
#include <QRegularExpression>

#include "configurator.h"
#include "constants.h"
#include "networkrequest.h"

QString Utility::getNumericConfigError(const QJsonObject& config,
                                       const QString& key,
                                       const QString& name,
                                       int lowerBound,
                                       int upperBound) {
  if (!config.contains(key) ||
      (config[key].isString() && config[key].toString().isEmpty())) {
    return QString(tr("Missing the value of '%1'.")).arg(name);
  } else {
    bool isConverted = false;
    int value        = config[key].toVariant().toInt(&isConverted);
    if (!isConverted) {
      return QString(tr("The value of '%1' seems invalid.")).arg(name);
    } else if (upperBound == -127 && value < lowerBound) {
      return QString(tr("The value of '%1' should above %2."))
        .arg(name, QString::number(lowerBound));
    } else if (value < lowerBound || value > upperBound) {
      return QString(tr("The value of '%1' should between %2 and %3."))
        .arg(name, QString::number(lowerBound), QString::number(upperBound));
    }
    return "";
  }
}

QString Utility::getStringConfigError(
  const QJsonObject& config,
  const QString& key,
  const QString& name,
  const QList<std::function<bool(const QString&)>>& checkpoints,
  bool allowEmpty,
  const QString& notPassedMsg) {
  if (allowEmpty && config[key].toString().isEmpty()) {
    return "";
  }
  if (!config.contains(key) || config[key].toString().isEmpty()) {
    return QString(tr("Missing the value of '%1'.")).arg(name);
  }
  if (checkpoints.size() > 0) {
    bool isMatched = false;
    for (std::function<bool(const QString&)> ckpt : checkpoints) {
      if (ckpt(config[key].toString())) {
        isMatched = true;
      }
    }
    if (!isMatched) {
      return notPassedMsg.arg(name);
    }
  }
  return "";
}

bool Utility::isIpAddrValid(const QString& ipAddr) {
  QRegularExpression ipAddrRegex(
    "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]"
    "|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
  return ipAddrRegex.match(ipAddr).hasMatch();
}

bool Utility::isIpAddrListValid(const QString& ipAddrList) {
  QStringList ips = ipAddrList.split(";");
  for (QString ip : ips) {
    if (!Utility::isIpAddrValid(ip.trimmed())) {
      return false;
    }
  }
  return true;
}

bool Utility::isDomainNameValid(const QString& domainName) {
  QRegularExpression domainNameRegex(
    "^(?:[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?\\.)+[a-z0-9][a-z0-9-]{0,61}[a-z0-"
    "9]$");
  return domainNameRegex.match(domainName).hasMatch();
}

bool Utility::isUrlValid(const QString& url) {
  QRegularExpression urlRegex(
    "^https?:\\/\\/"
    "(?:www\\.|(?!www))[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|www\\.["
    "a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|https?:\\/\\/"
    "(?:www\\.|(?!www))[a-zA-Z0-9]+\\.[^\\s]{2,}|www\\.[a-zA-Z0-9]+\\.[^\\s]{2,"
    "}$",
    QRegularExpression::CaseInsensitiveOption);
  return urlRegex.match(url).hasMatch();
}

bool Utility::isFileExists(const QString& filePath) {
  return QDir(filePath).exists();
}

bool Utility::isServerNameNotUsed(const QString& serverName) {
  Configurator& configurator(Configurator::getInstance());
  QJsonArray servers = configurator.getServers();

  for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
    QJsonObject server  = (*itr).toObject();
    QString _serverName = server["name"].toString();
    if (serverName == _serverName) {
      return false;
    }
  }
  return true;
}

QStringList Utility::getAlpn(const QString& alpn) {
  QStringList alpns;
  for (QString a : alpn.split(";")) {
    if (a.isEmpty()) {
      continue;
    }
    alpns.append(a.trimmed());
  }
  return alpns;
}

bool Utility::isAlpnValid(const QString& alpn) {
  static const QStringList ALPN_CANDIDATES = {"h2", "http/1.1"};
  QStringList alpns                        = getAlpn(alpn);
  for (QString a : alpns) {
    if (!ALPN_CANDIDATES.contains(a)) {
      return false;
    }
  }
  return true;
}

QString Utility::formatV2RayLog(const QString& log) {
  int timeStart  = log.indexOf("time=");
  int levelStart = log.indexOf("level=");
  int msgStart   = log.indexOf("msg=");

  QString time = log.mid(timeStart + 6, levelStart - timeStart - 14)
                   .replace('-', '/')
                   .replace('T', ' ');
  QString level = log.mid(levelStart + 6, msgStart - levelStart - 7);
  QString msg   = log.mid(msgStart + 5, log.size() - msgStart - 6);
  if (time.size() == 0) {
    return "";
  }
  return QString("%1 [%2] clash: %3").arg(time, level, msg);
}

QString Utility::getLatestRelease(const QString& releaseUrl,
                                  const QNetworkProxy* proxy) {
  QByteArray releaseJsonStr =
    NetworkRequest::getNetworkResponse(releaseUrl, proxy, HTTP_GET_TIMEOUT);
  QJsonObject latestRelease;
  QJsonDocument releaseJsonDoc = QJsonDocument::fromJson(releaseJsonStr);
  QJsonArray releases          = releaseJsonDoc.array();
  for (int i = 0; i < releases.size(); ++i) {
    QJsonObject release = releases[i].toObject();
    latestRelease       = release;
    break;
  }
  return latestRelease.empty() ? "" : latestRelease["name"].toString();
}

bool Utility::isVersionNewer(const QString& currentVersion,
                             const QString& version) {
  QList<int> _currentVersion = getVersion(currentVersion);
  QList<int> _version        = getVersion(version);

  for (int i = 0; i < _version.size() && i < _currentVersion.size(); ++i) {
    if (_version.at(i) > _currentVersion.at(i)) {
      return true;
    } else if (_version.at(i) < _currentVersion.at(i)) {
      return false;
    }
  }
  return false;
}

QList<int> Utility::getVersion(QString version) {
  QList<int> _version;
  if (version.startsWith('v') || version.startsWith('v')) {
    version = version.mid(1);
  }
  for (QString v : version.split('.')) {
    _version.append(v.toInt());
  }
  return _version;
}
