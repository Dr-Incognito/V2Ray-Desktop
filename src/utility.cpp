#include "utility.h"

#include <functional>

#include <QDebug>
#include <QDir>
#include <QList>
#include <QRegularExpression>

#include "configurator.h"

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
  const QString& notPassedMsg) {
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
  QStringList ips = ipAddrList.split(",");
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
    "^https?://(-\\.)?([^\\s/?\\.#-]+\\.?)+(/[^\\s]*)?$",
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
    QString _serverName = server["serverName"].toString();
    if (serverName == _serverName) {
      return false;
    }
  }
  return true;
}
