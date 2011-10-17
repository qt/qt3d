TARGET = qtga
include(../../qpluginbase.pri)
HEADERS += qtgahandler.h \
    qtgafile.h
SOURCES += main.cpp \
    qtgahandler.cpp \
    qtgafile.cpp

DESTDIR = $$QT.qt3d.plugins/imageformats
target.path = $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
