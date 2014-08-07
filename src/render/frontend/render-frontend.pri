INCLUDEPATH += $$PWD

include(framegraph-components/framegraph-components.pri)

HEADERS += \
    $$PWD/qmaterial.h \
    $$PWD/qmesh.h \
    $$PWD/qrenderpass.h \
    $$PWD/qshaderprogram.h \
    $$PWD/qshaderprogram_p.h \
    $$PWD/qtechnique.h \
    $$PWD/tag.h \
    $$PWD/qabstractshapemesh.h \
    $$PWD/qitemmodelbuffer.h \
    $$PWD/texture.h \
    $$PWD/sphere.h \
    $$PWD/qabstractscene.h \
    $$PWD/qeffect.h \
    $$PWD/qscene.h \
    $$PWD/qcriterion.h \
    $$PWD/qparameter.h \
    $$PWD/qparameter_p.h \
    $$PWD/qparametermapper.h \
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
    $$PWD/qrenderpass_p.h \
    $$PWD/qlayer.h \
    $$PWD/qlayer_p.h \
    $$PWD/qparametermapper_p.h \
    $$PWD/qopenglfilter.h \
    $$PWD/qplanemesh.h \
    $$PWD/qplanemesh_p.h \
    $$PWD/qcuboidmesh.h \
    $$PWD/qcuboidmesh_p.h \
    $$PWD/qdrawstate.h \
    $$PWD/qblendstate.h \
    $$PWD/qblendequation.h

SOURCES += \
    $$PWD/qmaterial.cpp \
    $$PWD/qmesh.cpp \
    $$PWD/qrenderpass.cpp \
    $$PWD/qshaderprogram.cpp \
    $$PWD/qtechnique.cpp \
    $$PWD/qabstractshapemesh.cpp \
    $$PWD/qitemmodelbuffer.cpp \
    $$PWD/texture.cpp \
    $$PWD/sphere.cpp \
    $$PWD/qabstractscene.cpp \
    $$PWD/qeffect.cpp \
    $$PWD/qscene.cpp \
    $$PWD/qcriterion.cpp \
    $$PWD/qparameter.cpp \
    $$PWD/qparametermapper.cpp \
    $$PWD/qtorusmesh.cpp \
    $$PWD/qspheremesh.cpp \
    $$PWD/qabstractlight.cpp \
    $$PWD/qpointlight.cpp \
    $$PWD/qspotlight.cpp \
    $$PWD/qdirectionallight.cpp \
    $$PWD/qlayer.cpp \
    $$PWD/qopenglfilter.cpp \
    $$PWD/qplanemesh.cpp \
    $$PWD/qcuboidmesh.cpp \
    $$PWD/qdrawstate.cpp \
    $$PWD/qblendstate.cpp \
    $$PWD/qblendequation.cpp
