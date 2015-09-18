INCLUDEPATH += $$PWD

include(framegraph-components/framegraph-components.pri)

HEADERS += \
    $$PWD/qrenderaspect.h \
    $$PWD/qrenderaspect_p.h \
    $$PWD/qmaterial.h \
    $$PWD/qrenderpass.h \
    $$PWD/qshaderprogram.h \
    $$PWD/qshaderprogram_p.h \
    $$PWD/qtechnique.h \
    $$PWD/qitemmodelbuffer.h \
    $$PWD/qtexture.h \
    $$PWD/sphere.h \
    $$PWD/qabstractsceneloader.h \
    $$PWD/qeffect.h \
    $$PWD/qsceneloader.h \
    $$PWD/qparameter.h \
    $$PWD/qparameter_p.h \
    $$PWD/qabstractlight.h \
    $$PWD/qpointlight.h \
    $$PWD/qspotlight.h \
    $$PWD/qdirectionallight.h \
    $$PWD/qabstractlight_p.h \
    $$PWD/qspotlight_p.h \
    $$PWD/qdirectionallight_p.h \
    $$PWD/qpointlight_p.h \
    $$PWD/qmaterial_p.h \
    $$PWD/qeffect_p.h \
    $$PWD/qtechnique_p.h \
    $$PWD/qabstractsceneloader_p.h \
    $$PWD/qrenderpass_p.h \
    $$PWD/qlayer.h \
    $$PWD/qlayer_p.h \
    $$PWD/qopenglfilter.h \
    $$PWD/qrenderattachment.h \
    $$PWD/qrenderattachment_p.h \
    $$PWD/qrendertarget.h \
    $$PWD/qrendertarget_p.h \
    $$PWD/qannotation.h \
    $$PWD/qannotation_p.h \
    $$PWD/qparametermapping_p.h \
    $$PWD/qparametermapping.h \
    $$PWD/qshaderdata.h \
    $$PWD/qshaderdata_p.h \
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
    $$PWD/qmaterial.cpp \
    $$PWD/qrenderpass.cpp \
    $$PWD/qshaderprogram.cpp \
    $$PWD/qtechnique.cpp \
    $$PWD/qitemmodelbuffer.cpp \
    $$PWD/sphere.cpp \
    $$PWD/qabstractsceneloader.cpp \
    $$PWD/qeffect.cpp \
    $$PWD/qsceneloader.cpp \
    $$PWD/qparameter.cpp \
    $$PWD/qabstractlight.cpp \
    $$PWD/qpointlight.cpp \
    $$PWD/qspotlight.cpp \
    $$PWD/qdirectionallight.cpp \
    $$PWD/qlayer.cpp \
    $$PWD/qopenglfilter.cpp \
    $$PWD/qrenderattachment.cpp \
    $$PWD/qrendertarget.cpp \
    $$PWD/qannotation.cpp \
    $$PWD/qparametermapping.cpp \
    $$PWD/qshaderdata.cpp \
    $$PWD/qabstracttextureprovider.cpp \
    $$PWD/qwrapmode.cpp \
    $$PWD/qabstracttextureimage.cpp \
    $$PWD/qtextureimage.cpp \
    $$PWD/qtextureproviders.cpp \
    $$PWD/qwindow.cpp
