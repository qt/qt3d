TEMPLATE = app
TARGET = tst_matrix_properties
QT += declarative script testlib
macx:CONFIG -= app_bundle

SOURCES += tst_matrix_properties.cpp

symbian* {
    data.sources = data/*
    data.path = data
    DEPLOYMENT += data
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}




