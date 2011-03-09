load(qttest_p4.prf)
TEMPLATE=app
QT += testlib
CONFIG += unittest warn_on qt3d

INCLUDEPATH += ../../../../src/threed/textures
VPATH += ../../../../src/threed/textures

HEADERS += qglsharedresource_p.h
SOURCES += tst_qglsharedresource.cpp qglsharedresource.cpp
