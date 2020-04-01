#ifndef YAMLHELPER_H
#define YAMLHELPER_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QStringList>

class YamlHelper : public QObject {
  Q_OBJECT
 public:
  explicit YamlHelper(QObject* parent = nullptr){};
  static QString fromJsonObject(const QJsonObject& cfg);

 private:
  static const QString SPACING;
  static void fromJsonValue(const QJsonValue& value, QStringList& yamlItems);
  static void fromJsonObject(const QJsonObject& obj, QStringList& yamlItems);
  static void fromJsonArray(const QJsonArray& arr, QStringList& yamlItems);
};

#endif  // YAMLHELPER_H
