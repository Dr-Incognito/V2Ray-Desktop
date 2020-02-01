#ifndef QRCODEHELPER_H
#define QRCODEHELPER_H

#include <QImage>
#include <QObject>

class QrCodeHelper : public QObject {
  Q_OBJECT
 public:
  explicit QrCodeHelper(QObject* parent = nullptr);
  static QString decode(const QImage& img);

 signals:
};

#endif  // QRCODEHELPER_H
