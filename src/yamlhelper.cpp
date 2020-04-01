#include "yamlhelper.h"

const QString YamlHelper::SPACING = "  ";

QString YamlHelper::fromJsonObject(const QJsonObject &cfg) {
  QStringList yamlItems;
  fromJsonValue(QJsonValue(cfg), yamlItems);
  return yamlItems.join('\n');
}

void YamlHelper::fromJsonValue(const QJsonValue &value,
                               QStringList &yamlItems) {
  if (value.isObject()) {
    fromJsonObject(value.toObject(), yamlItems);
  } else if (value.isArray()) {
    fromJsonArray(value.toArray(), yamlItems);
  } else if (value.isString()) {
    QString v = value.toString();
    yamlItems.append(v);
  } else if (value.isBool()) {
    bool v = value.toBool();
    yamlItems.append(v ? "true" : "false");
  } else if (value.isDouble()) {
    // No double values in config
    int v = value.toInt();
    yamlItems.append(QString::number(v));
  } else if (value.isNull()) {
    yamlItems.append("null");
  }
}

void YamlHelper::fromJsonObject(const QJsonObject &obj,
                                QStringList &yamlItems) {
  for (auto itr = obj.begin(); itr != obj.end(); ++itr) {
    QStringList _yamlItems;
    QJsonValue value = itr.value();
    fromJsonValue(value, _yamlItems);

    if (value.isString() || value.isBool() || value.isDouble()) {
      yamlItems.append(QString("%1: %2").arg(itr.key(), _yamlItems.at(0)));
    } else {
      yamlItems.append(QString("%1:").arg(itr.key()));
      for (int i = 0; i < _yamlItems.size(); ++i) {
        yamlItems.append(SPACING + _yamlItems.at(i));
      }
    }
  }
}

void YamlHelper::fromJsonArray(const QJsonArray &arr, QStringList &yamlItems) {
  if (arr.size() == 0) {
    yamlItems.append("[]");
  }
  for (int i = 0; i < arr.size(); ++i) {
    QStringList _yamlItems;
    fromJsonValue(arr.at(i), _yamlItems);
    for (int j = 0; j < _yamlItems.size(); ++j) {
      yamlItems.append((j == 0 ? "- " : SPACING) + _yamlItems.at(j));
    }
  }
}
