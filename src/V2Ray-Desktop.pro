QT += quick quickcontrols2 widgets svg

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QUAZIP_STATIC

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    appproxy.h \
    appproxyworker.h \
    configurator.h \
    constants.h \
    networkproxy.h \
    networkrequest.h \
    qrcodehelper.h \
    runguard.h \
    serverconfighelper.h \
    utility.h \
    v2raycore.h

SOURCES += \
    appproxy.cpp \
    appproxyworker.cpp \
    configurator.cpp \
    main.cpp \
    networkproxy.cpp \
    networkrequest.cpp \
    qrcodehelper.cpp \
    runguard.cpp \
    serverconfighelper.cpp \
    utility.cpp \
    v2raycore.cpp

include(3rdparty/qzxing/src/QZXing.pri)
LIBS += -lz

RESOURCES += qml.qrc

TRANSLATIONS += locales/zh-CN.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    RC_ICONS   = images/v2ray.ico
}
unix {
    RC_ICONS   = images/v2ray.ico
}
osx {
    ICON       = images/v2ray.icns
}
