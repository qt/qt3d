// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <Qt3DCore/QEntity>

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/qaspectengine.h>

#include <QGuiApplication>

#include "gbuffer.h"
#include "deferredrenderer.h"
#include "finaleffect.h"
#include "sceneeffect.h"
#include "pointlightblock.h"
#include "sceneentity.h"
#include "screenquadentity.h"

#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    Qt3DExtras::Qt3DWindow view;

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Scene Content
    SceneEntity *sceneContent = new SceneEntity(rootEntity);

    // Screen Quad
    ScreenQuadEntity *screenQuad = new ScreenQuadEntity(rootEntity);

    // Shared Components
    Qt3DRender::QLayer *sceneLayer = sceneContent->layer();
    Qt3DRender::QLayer *quadLayer = screenQuad->layer();
    SceneEffect *sceneEffect = sceneContent->effect();
    FinalEffect *finalEffect = screenQuad->effect();

    // Scene Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->setFieldOfView(45.0f);
    camera->setNearPlane(0.01f);
    camera->setFarPlane(1000.0f);
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setPosition(QVector3D(10.0f, 10.0f, -25.0f));
    camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 10.0f));

    // For camera controls
    Qt3DExtras::QFirstPersonCameraController *camController = new Qt3DExtras::QFirstPersonCameraController(rootEntity);
    camController->setCamera(camera);

    // FrameGraph
    DeferredRenderer *deferredRenderer = new DeferredRenderer();
    deferredRenderer->setNormalizedRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
    deferredRenderer->setFinalPassCriteria(finalEffect->passCriteria());
    deferredRenderer->setGeometryPassCriteria(sceneEffect->passCriteria());
    deferredRenderer->setSceneCamera(camera);
    deferredRenderer->setGBufferLayer(sceneLayer);
    deferredRenderer->setScreenQuadLayer(quadLayer);
    deferredRenderer->setSurface(&view);

    view.setActiveFrameGraph(deferredRenderer);

    // Set root object of the scene
    view.setRootEntity(rootEntity);
    // Show window
    view.show();

    return app.exec();
}
