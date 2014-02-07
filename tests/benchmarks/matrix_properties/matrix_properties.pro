TEMPLATE = app
TARGET = tst_matrix_properties
QT += qml quick testlib
macx:CONFIG -= app_bundle

SOURCES += tst_matrix_properties.cpp

DEFINES += SRCDIR=\\\"$$PWD\\\"




DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
