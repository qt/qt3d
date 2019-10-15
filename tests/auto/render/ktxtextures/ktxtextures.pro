TEMPLATE = app

TARGET = tst_ktxtextures

CONFIG += testcase

SOURCES += tst_ktxtextures.cpp

TESTDATA = data/*

QT += core-private 3dcore 3dcore-private 3drender 3drender-private testlib
