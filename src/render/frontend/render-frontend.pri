INCLUDEPATH += $$PWD

include(framegraph-components/framegraph-components.pri)

HEADERS += \
    $$PWD/qrenderaspect.h \
    $$PWD/qrenderaspect_p.h \
    $$PWD/qitemmodelbuffer.h \
    $$PWD/qtexture.h \
    $$PWD/sphere.h \
    $$PWD/qabstractsceneloader.h \
    $$PWD/qsceneloader.h \
    $$PWD/qabstractlight.h \
    $$PWD/qpointlight.h \
    $$PWD/qspotlight.h \
    $$PWD/qdirectionallight.h \
    $$PWD/qabstractlight_p.h \
    $$PWD/qspotlight_p.h \
    $$PWD/qdirectionallight_p.h \
    $$PWD/qpointlight_p.h \
    $$PWD/qabstractsceneloader_p.h \
    $$PWD/qlayer.h \
    $$PWD/qlayer_p.h \
    $$PWD/qrenderattachment.h \
    $$PWD/qrenderattachment_p.h \
    $$PWD/qrendertarget.h \
    $$PWD/qrendertarget_p.h \
    $$PWD/qabstracttextureprovider.h \
    $$PWD/qabstracttextureprovider_p.h \
    $$PWD/qwrapmode.h \
    $$PWD/qabstracttextureimage.h \
    $$PWD/qabstracttextureimage_p.h \
    $$PWD/qtextureimage.h \
    $$PWD/qtextureproviders.h \
    $$PWD/qwindow.h \
    $$PWD/qwindow_p.h

SOURCES += \
    $$PWD/qrenderaspect.cpp \
    $$PWD/qitemmodelbuffer.cpp \
    $$PWD/sphere.cpp \
    $$PWD/qabstractsceneloader.cpp \
    $$PWD/qsceneloader.cpp \
    $$PWD/qabstractlight.cpp \
    $$PWD/qpointlight.cpp \
    $$PWD/qspotlight.cpp \
    $$PWD/qdirectionallight.cpp \
    $$PWD/qlayer.cpp \
    $$PWD/qrenderattachment.cpp \
    $$PWD/qrendertarget.cpp \
    $$PWD/qabstracttextureprovider.cpp \
    $$PWD/qwrapmode.cpp \
    $$PWD/qabstracttextureimage.cpp \
    $$PWD/qtextureimage.cpp \
    $$PWD/qtextureproviders.cpp \
    $$PWD/qwindow.cpp
