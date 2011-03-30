TEMPLATE = app
TARGET = lander
CONFIG += qt warn_on
QT += declarative

DESTDIR = ../../bin

qmlFiles.sources = qml
DEPLOYMENT += qmlFiles

SOURCES += main.cpp
