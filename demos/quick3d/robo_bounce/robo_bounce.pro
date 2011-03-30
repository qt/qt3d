TEMPLATE = app
TARGET = robo_bounce
CONFIG += qt warn_on
QT += declarative

DESTDIR = ../../bin

qmlFiles.sources = qml
DEPLOYMENT += qmlFiles

SOURCES += main.cpp
