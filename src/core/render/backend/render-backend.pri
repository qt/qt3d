INCLUDEPATH += $$PWD

include("framegraph/framegraph.pri")
include("jobs/render-jobs.pri")

HEADERS += \
    $$PWD/rendereraspect.h \
    $$PWD/renderthread.h \
    $$PWD/renderconfiguration.h \
    $$PWD/renderdevice.h \
    $$PWD/renderer.h \
    $$PWD/rendermaterial.h \
    $$PWD/drawable.h \
    $$PWD/rendermesh.h \
    $$PWD/qgraphicscontext.h \
    $$PWD/renderbin.h \
    $$PWD/rendershader.h \
    $$PWD/rendertechnique.h \
    $$PWD/rendercamera.h \
    $$PWD/quniformvalue.h \
    $$PWD/rendernode.h \
    $$PWD/renderscenebuilder.h \
    $$PWD/rendertexture.h \
    $$PWD/rendertextureprovider.h \
    $$PWD/meshmanager.h \
    $$PWD/drawstate.h \
    $$PWD/states/blendstate.h \
    $$PWD/genericstate.h


SOURCES += \
    $$PWD/rendereraspect.cpp \
    $$PWD/renderthread.cpp \
    $$PWD/renderconfiguration.cpp \
    $$PWD/renderdevice.cpp \
    $$PWD/renderer.cpp \
    $$PWD/rendermaterial.cpp \
    $$PWD/drawable.cpp \
    $$PWD/rendermesh.cpp \
    $$PWD/qgraphicscontext.cpp \
    $$PWD/renderbin.cpp \
    $$PWD/rendershader.cpp \
    $$PWD/rendertechnique.cpp \
    $$PWD/rendercamera.cpp \
    $$PWD/quniformvalue.cpp \
    $$PWD/rendernode.cpp \
    $$PWD/renderscenebuilder.cpp \
    $$PWD/rendertexture.cpp \
    $$PWD/rendertextureprovider.cpp \
    $$PWD/meshmanager.cpp \
    $$PWD/drawstate.cpp \
    $$PWD/states/blendstate.cpp
