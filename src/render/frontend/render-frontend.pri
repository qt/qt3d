INCLUDEPATH += $$PWD

include(framegraph-components/framegraph-components.pri)

HEADERS += \
    $$PWD/qmaterial.h \
    $$PWD/qmesh.h \
    $$PWD/qrenderpass.h \
    $$PWD/shaderprogram.h \
    $$PWD/qtechnique.h \
    $$PWD/tag.h \
    $$PWD/qabstractshapemesh.h \
    $$PWD/qitemmodelbuffer.h \
    $$PWD/texture.h \
    $$PWD/sphere.h \
    $$PWD/qabstractscene.h \
    $$PWD/qeffect.h \
    $$PWD/qscene.h \
    $$PWD/techniquecriterion.h \
    $$PWD/renderpasscriterion.h \
    $$PWD/parameter.h \
    $$PWD/parametermapper.h \
    $$PWD/qtorusmesh.h \
    $$PWD/qspheremesh.h \
    $$PWD/qabstractshapemesh_p.h \
    $$PWD/qmesh_p.h \
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
    $$PWD/qabstractscene_p.h \
    $$PWD/qrenderpass_p.h

SOURCES += \
    $$PWD/qmaterial.cpp \
    $$PWD/qmesh.cpp \
    $$PWD/qrenderpass.cpp \
    $$PWD/shaderprogram.cpp \
    $$PWD/qtechnique.cpp \
    $$PWD/qabstractshapemesh.cpp \
    $$PWD/qitemmodelbuffer.cpp \
    $$PWD/texture.cpp \
    $$PWD/sphere.cpp \
    $$PWD/qabstractscene.cpp \
    $$PWD/qeffect.cpp \
    $$PWD/qscene.cpp \
    $$PWD/techniquecriterion.cpp \
    $$PWD/renderpasscriterion.cpp \
    $$PWD/parameter.cpp \
    $$PWD/parametermapper.cpp \
    $$PWD/qtorusmesh.cpp \
    $$PWD/qspheremesh.cpp \
    $$PWD/qabstractlight.cpp \
    $$PWD/qpointlight.cpp \
    $$PWD/qspotlight.cpp \
    $$PWD/qdirectionallight.cpp
