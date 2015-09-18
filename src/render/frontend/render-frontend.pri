INCLUDEPATH += $$PWD

include(framegraph-components/framegraph-components.pri)

HEADERS += \
    $$PWD/qrenderaspect.h \
    $$PWD/qrenderaspect_p.h \
    $$PWD/qitemmodelbuffer.h \
    $$PWD/sphere.h \
    $$PWD/qabstractsceneloader.h \
    $$PWD/qsceneloader.h \
    $$PWD/qabstractsceneloader_p.h \
    $$PWD/qlayer.h \
    $$PWD/qlayer_p.h \
    $$PWD/qrenderattachment.h \
    $$PWD/qrenderattachment_p.h \
    $$PWD/qrendertarget.h \
    $$PWD/qrendertarget_p.h \
    $$PWD/qwindow.h \
    $$PWD/qwindow_p.h

SOURCES += \
    $$PWD/qrenderaspect.cpp \
    $$PWD/qitemmodelbuffer.cpp \
    $$PWD/sphere.cpp \
    $$PWD/qabstractsceneloader.cpp \
    $$PWD/qsceneloader.cpp \
    $$PWD/qlayer.cpp \
    $$PWD/qrenderattachment.cpp \
    $$PWD/qrendertarget.cpp \
    $$PWD/qwindow.cpp
