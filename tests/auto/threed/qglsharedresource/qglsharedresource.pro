TARGET = tst_qglsharedresource
CONFIG += testcase
TEMPLATE=app
QT += testlib
CONFIG += warn_on qt3d

INCLUDEPATH += ../../../../src/threed/textures
VPATH += ../../../../src/threed/textures

HEADERS += qglsharedresource_p.h
SOURCES += tst_qglsharedresource.cpp qglsharedresource.cpp
QT+=widgets
