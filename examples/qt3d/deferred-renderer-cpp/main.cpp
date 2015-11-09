/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <Qt3DCore/QEntity>
#include <window.h>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QFrameGraph>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QSphereMesh>
#include <Qt3DRender/QPlaneMesh>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QParameter>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QWindow>
#include <Qt3DCore/qaspectengine.h>

#include <QGuiApplication>

#include "gbuffer.h"
#include "deferredrenderer.h"
#include "finaleffect.h"
#include "sceneeffect.h"
#include "pointlightblock.h"

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    Qt3DRender::QWindow view;
    Qt3DInput::QInputAspect *input = new Qt3DInput::QInputAspect();
    view.registerAspect(input);

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    rootEntity->setObjectName(QStringLiteral("rootEntity"));

    // Shared Components
    GBuffer *gBuffer = new GBuffer();
    FinalEffect *finalEffect = new FinalEffect();
    SceneEffect *sceneEffect = new SceneEffect();

    Qt3DRender::QSphereMesh *sphereMesh = new Qt3DRender::QSphereMesh();
    sphereMesh->setRings(50);
    sphereMesh->setSlices(100);

    Qt3DRender::QLayer *sceneLayer = new Qt3DRender::QLayer();
    sceneLayer->setNames(QStringList("scene"));

    Qt3DRender::QLayer *quadLayer = new Qt3DRender::QLayer();
    quadLayer->setNames(QStringList("screenQuad"));

    Qt3DRender::QPlaneMesh *planeMesh = new Qt3DRender::QPlaneMesh();
    planeMesh->setMeshResolution(QSize(2, 2));
    planeMesh->setWidth(2.0f);
    planeMesh->setHeight(2.0f);

    Qt3DRender::QPointLight *light1 = new Qt3DRender::QPointLight();
    light1->setColor(Qt::white);
    light1->setIntensity(1.5f);

    rootEntity->addComponent(light1);

    // Scene Camera
    Qt3DCore::QCamera *camera = view.defaultCamera();

    camera->setFieldOfView(45.0f);
    camera->setNearPlane(0.01f);
    camera->setFarPlane(1000.0f);
    camera->setProjectionType(Qt3DCore::QCameraLens::PerspectiveProjection);

    camera->setPosition(QVector3D(10.0f, 10.0f, -25.0f));
    camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 10.0f));

    input->setCamera(camera);

    // FrameGraph
    Qt3DRender::QFrameGraph *frameGraph = new Qt3DRender::QFrameGraph();
    DeferredRenderer *deferredRenderer = new DeferredRenderer();
    deferredRenderer->setClearColor(Qt::black);
    deferredRenderer->setRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
    deferredRenderer->setFinalPassCriteria(finalEffect->passCriteria());
    deferredRenderer->setGeometryPassCriteria(sceneEffect->passCriteria());
    deferredRenderer->setGBuffer(gBuffer);
    deferredRenderer->setSceneCamera(camera);
    deferredRenderer->setGBufferLayers(sceneLayer->names());
    deferredRenderer->setScreenQuadLayers(quadLayer->names());

    frameGraph->setActiveFrameGraph(deferredRenderer);
    view.setFrameGraph(frameGraph);

    // Scene Content
    Qt3DCore::QEntity *sphereOne = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QMaterial *sphereOneMaterial = new Qt3DRender::QMaterial();

    sphereOneMaterial->setEffect(sceneEffect);
    sphereOneMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("meshColor"), QColor(Qt::blue)));

    Qt3DCore::QTransform *sphereOneTransform = new Qt3DCore::QTransform;
    sphereOneTransform->setTranslation(QVector3D(-10.0f, 0.0f, 25.0f));

    Qt3DRender::QPointLight *light2 = new Qt3DRender::QPointLight();
    light2->setColor(Qt::white);
    light2->setIntensity(1.5f);

    sphereOne->addComponent(sphereOneTransform);
    sphereOne->addComponent(sphereOneMaterial);
    sphereOne->addComponent(sphereMesh);
    sphereOne->addComponent(sceneLayer);
    sphereOne->addComponent(light2);

    Qt3DCore::QEntity *sphereTwo = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QMaterial *sphereTwoMaterial = new Qt3DRender::QMaterial();
    sphereTwoMaterial->setEffect(sceneEffect);
    sphereTwoMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("meshColor"), QColor(Qt::yellow)));

    Qt3DRender::QPointLight *light3 = new Qt3DRender::QPointLight();
    light3->setColor(Qt::blue);
    light3->setIntensity(0.5f);

    sphereTwo->addComponent(sphereMesh);
    sphereTwo->addComponent(sceneLayer);
    sphereTwo->addComponent(sphereTwoMaterial);
    sphereTwo->addComponent(light3);

    // Screen Quad
    Qt3DCore::QEntity *screenQuad = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QMaterial *screenQuadMaterial = new Qt3DRender::QMaterial();
    screenQuadMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("position"), gBuffer->positionTexture()));
    screenQuadMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("normal"), gBuffer->normalTexture()));
    screenQuadMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("color"), gBuffer->colorTexture()));
    screenQuadMaterial->addParameter(new Qt3DRender::QParameter(QStringLiteral("winSize"), QSize(1024, 1024)));
    PointLightBlock *lightsData = new PointLightBlock(screenQuadMaterial);
    lightsData->addLight(light1);
    lightsData->addLight(light2);
    lightsData->addLight(light3);

    finalEffect->gl3Technique()->addParameter(new Qt3DRender::QParameter(QStringLiteral("PointLightBlock"), QVariant::fromValue(lightsData)));
    finalEffect->gl2Technique()->addParameter(new Qt3DRender::QParameter(QStringLiteral("pointLights"), QVariant::fromValue(lightsData)));
    screenQuadMaterial->setEffect(finalEffect);

    Qt3DCore::QTransform *screenPlaneTransform = new Qt3DCore::QTransform;
    screenPlaneTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 90.0f));

    screenQuad->addComponent(quadLayer);
    screenQuad->addComponent(screenPlaneTransform);
    screenQuad->addComponent(screenQuadMaterial);
    screenQuad->addComponent(planeMesh);

    // Set root object of the scene
    view.setRootEntity(rootEntity);
    // Show window
    view.show();

    return app.exec();
}
