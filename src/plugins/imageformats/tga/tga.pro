TARGET = qtga
include(../../qpluginbase.pri)
HEADERS += qtgahandler.h \
    qtgafile.h
SOURCES += main.cpp \
    qtgahandler.cpp \
    qtgafile.cpp
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
