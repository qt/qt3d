TARGET = tst_qglsharedresource
CONFIG += testcase
TEMPLATE=app
QT += testlib qt3d opengl widgets
CONFIG += warn_on

INCLUDEPATH += ../../../../src/threed/textures
VPATH += ../../../../src/threed/textures

HEADERS += qglsharedresource_p.h
SOURCES += tst_qglsharedresource.cpp qglsharedresource.cpp
QT+=widgets
