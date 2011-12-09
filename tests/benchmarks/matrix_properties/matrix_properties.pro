TEMPLATE = app
TARGET = tst_matrix_properties
QT += declarative quick testlib
macx:CONFIG -= app_bundle

SOURCES += tst_matrix_properties.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"




