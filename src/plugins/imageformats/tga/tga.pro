TARGET = qtga
include(../../qpluginbase.pri)
include(../../../../pkg.pri)
!package: DESTDIR = $$QT3D_INSTALL_PLUGINS/imageformats
HEADERS += qtgahandler.h \
    qtgafile.h
SOURCES += main.cpp \
    qtgahandler.cpp \
    qtgafile.cpp
target.path += $$QT3D_INSTALL_PLUGINS/imageformats
INSTALLS += target
