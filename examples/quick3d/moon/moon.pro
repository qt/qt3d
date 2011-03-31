TEMPLATE = app
TARGET = moon
CONFIG += qt warn_on

SOURCES += main.cpp
HEADERS += qmlres.h

QT += declarative

DESTDIR = ../../../bin/$$TARGET

qmlResources.files = qml
symbian {
    DEPLOYMENT = qmlResources
} else {
    macx {
        qmlResources.path = Contents/Resources
        QMAKE_BUNDLE_DATA += qmlResources
    } else {
        qmlResources.path = $$OUT_PWD/../../../bin/$$TARGET.qml
        INSTALLS += qmlResources
    }
}
