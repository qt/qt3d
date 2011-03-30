TEMPLATE = app
TARGET = bounce
CONFIG += qt warn_on
QT += declarative

DESTDIR = ../../bin

qmlFiles.sources = qml
DEPLOYMENT += qmlFiles

SOURCES += main.cpp
