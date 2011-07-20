TEMPLATE = app
TARGET = tst_matrix_properties
QT += declarative script testlib
macx:CONFIG -= app_bundle

SOURCES += tst_matrix_properties.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"




