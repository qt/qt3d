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
    $$PWD/rendermesh.h \
    $$PWD/qgraphicscontext.h \
    $$PWD/rendershader.h \
    $$PWD/rendertechnique.h \
    $$PWD/rendercameralens.h \
    $$PWD/quniformvalue.h \
    $$PWD/renderscenebuilder.h \
    $$PWD/rendertexture.h \
    $$PWD/rendertextureprovider.h \
    $$PWD/meshdatamanager.h \
    $$PWD/drawstate.h \
    $$PWD/states/blendstate.h \
    $$PWD/genericstate.h \
    $$PWD/qgraphicshelperinterface.h \
    $$PWD/qgraphicshelpergl3.h \
    $$PWD/qgraphicshelperes2.h \
    $$PWD/qgraphicshelpergl2.h \
    $$PWD/renderview.h \
    $$PWD/rendercommand.h \
    $$PWD/cameramanager.h \
    $$PWD/renderqueues.h \
    $$PWD/materialmanager.h \
    $$PWD/meshmanager.h \
    $$PWD/matrixmanager.h \
    $$PWD/vaomanager.h \
    $$PWD/shadermanager.h \
    $$PWD/techniquemanager.h \
    $$PWD/rendereffect.h \
    $$PWD/effectmanager.h \
    $$PWD/renderrenderpass.h \
    $$PWD/renderpassmanager.h \
    $$PWD/parameterpack.h \
    $$PWD/texturemanager.h \
    $$PWD/texturedatamanager.h \
    $$PWD/renderlayer.h \
    $$PWD/layermanager.h \
    $$PWD/renderlight.h \
    $$PWD/lightmanager.h \
    $$PWD/renderentity.h \
    $$PWD/entitymanager.h \
    $$PWD/rendercriterion.h \
    $$PWD/criterionmanager.h \
    $$PWD/rendersceneobserver.h

SOURCES += \
    $$PWD/rendereraspect.cpp \
    $$PWD/renderthread.cpp \
    $$PWD/renderconfiguration.cpp \
    $$PWD/renderdevice.cpp \
    $$PWD/renderer.cpp \
    $$PWD/rendermaterial.cpp \
    $$PWD/rendermesh.cpp \
    $$PWD/qgraphicscontext.cpp \
    $$PWD/rendershader.cpp \
    $$PWD/rendertechnique.cpp \
    $$PWD/rendercameralens.cpp \
    $$PWD/quniformvalue.cpp \
    $$PWD/renderscenebuilder.cpp \
    $$PWD/rendertexture.cpp \
    $$PWD/rendertextureprovider.cpp \
    $$PWD/meshdatamanager.cpp \
    $$PWD/drawstate.cpp \
    $$PWD/states/blendstate.cpp \
    $$PWD/qgraphicshelpergl3.cpp \
    $$PWD/qgraphicshelperes2.cpp \
    $$PWD/qgraphicshelpergl2.cpp \
    $$PWD/renderview.cpp \
    $$PWD/rendercommand.cpp \
    $$PWD/cameramanager.cpp \
    $$PWD/renderqueues.cpp \
    $$PWD/materialmanager.cpp \
    $$PWD/meshmanager.cpp \
    $$PWD/matrixmanager.cpp \
    $$PWD/vaomanager.cpp \
    $$PWD/shadermanager.cpp \
    $$PWD/techniquemanager.cpp \
    $$PWD/rendereffect.cpp \
    $$PWD/effectmanager.cpp \
    $$PWD/renderrenderpass.cpp \
    $$PWD/renderpassmanager.cpp \
    $$PWD/parameterpack.cpp \
    $$PWD/texturemanager.cpp \
    $$PWD/texturedatamanager.cpp \
    $$PWD/renderlayer.cpp \
    $$PWD/layermanager.cpp \
    $$PWD/renderlight.cpp \
    $$PWD/lightmanager.cpp \
    $$PWD/renderentity.cpp \
    $$PWD/entitymanager.cpp \
    $$PWD/rendercriterion.cpp \
    $$PWD/criterionmanager.cpp \
    $$PWD/rendersceneobserver.cpp
