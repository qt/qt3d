INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    network/qdownloadmanager.h \
    network/qabstractdownloadmanager.h \
    network/qthreadeddownloadmanager.h
SOURCES += \
    qdownloadmanager.cpp \
    network/qthreadeddownloadmanager.cpp \
    network/qabstractdownloadmanager.cpp
PRIVATE_HEADERS += \
    network/qthreadeddownloadmanager_p.h
