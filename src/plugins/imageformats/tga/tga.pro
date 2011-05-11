TARGET = qtga
include(../../qpluginbase.pri)
!package: DESTDIR = $$[QT_INSTALL_PLUGINS]/imageformats
HEADERS += qtgahandler.h \
    qtgafile.h
SOURCES += main.cpp \
    qtgahandler.cpp \
    qtgafile.cpp
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
symbian {
    TARGET.UID3 = 0x20031E99
}
