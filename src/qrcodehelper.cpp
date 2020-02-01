#include "qrcodehelper.h"

#include "3rdparty/qzxing/src/QZXing.h"

QrCodeHelper::QrCodeHelper(QObject* parent) : QObject(parent) {}

QString QrCodeHelper::decode(const QImage& img) {
  QZXing decoder;
  decoder.setDecoder(QZXing::DecoderFormat_QR_CODE |
                     QZXing::DecoderFormat_EAN_13);
  return decoder.decodeImage(img);
}
