TEMPLATE = app
TARGET = basket_qml
CONFIG += qt warn_on
QT += declarative

DESTDIR = ../../bin

qmlFiles.sources = qml
DEPLOYMENT += qmlFiles

SOURCES += main.cpp
