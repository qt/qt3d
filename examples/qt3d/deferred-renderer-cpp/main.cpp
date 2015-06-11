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

#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QMaterial>
#include <Qt3DRenderer/QSphereMesh>
#include <Qt3DRenderer/QPlaneMesh>
#include <Qt3DRenderer/QLayer>
#include <Qt3DRenderer/QParameter>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QLookAtTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QPointLight>
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

    Window view;
    Qt3D::QAspectEngine engine;
    engine.registerAspect(new Qt3D::QRenderAspect());
    Qt3D::QInputAspect *input = new Qt3D::QInputAspect;
    engine.registerAspect(input);
    engine.initialize();
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.setData(data);

    // Root entity
    Qt3D::QEntity *rootEntity = new Qt3D::QEntity();
    rootEntity->setObjectName(QStringLiteral("rootEntity"));

    // Shared Components
    GBuffer *gBuffer = new GBuffer();
    FinalEffect *finalEffect = new FinalEffect();
    SceneEffect *sceneEffect = new SceneEffect();

    Qt3D::QSphereMesh *sphereMesh = new Qt3D::QSphereMesh();
    sphereMesh->setRings(50);
    sphereMesh->setSlices(100);

    Qt3D::QLayer *sceneLayer = new Qt3D::QLayer();
    sceneLayer->setNames(QStringList("scene"));

    Qt3D::QLayer *quadLayer = new Qt3D::QLayer();
    quadLayer->setNames(QStringList("screenQuad"));

    Qt3D::QPlaneMesh *planeMesh = new Qt3D::QPlaneMesh();
    planeMesh->setMeshResolution(QSize(2, 2));
    planeMesh->setWidth(2.0f);
    planeMesh->setHeight(2.0f);

    Qt3D::QPointLight *light1 = new Qt3D::QPointLight();
    light1->setColor(Qt::white);
    light1->setIntensity(1.5f);

    rootEntity->addComponent(light1);

    // Scene Camera
    Qt3D::QCamera *camera = new Qt3D::QCamera();

    camera->setAspectRatio(16.0f/9.0f);
    camera->setFieldOfView(45.0f);
    camera->setNearPlane(0.01f);
    camera->setFarPlane(1000.0f);
    camera->setProjectionType(Qt3D::QCameraLens::PerspectiveProjection);

    camera->setPosition(QVector3D(10.0f, 10.0f, -25.0f));
    camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 10.0f));

    input->setCamera(camera);

    // FrameGraph
    Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
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
    rootEntity->addComponent(frameGraph);

    // Scene Content
    Qt3D::QEntity *sphereOne = new Qt3D::QEntity(rootEntity);
    Qt3D::QMaterial *sphereOneMaterial = new Qt3D::QMaterial();

    sphereOneMaterial->setEffect(sceneEffect);
    sphereOneMaterial->addParameter(new Qt3D::QParameter(QStringLiteral("meshColor"), QColor(Qt::blue)));

    Qt3D::QTranslateTransform *sphereOneTranslate = new Qt3D::QTranslateTransform();
    sphereOneTranslate->setDx(-10.0f);
    sphereOneTranslate->setDy(0.0f);
    sphereOneTranslate->setDz(25.0f);

    Qt3D::QPointLight *light2 = new Qt3D::QPointLight();
    light2->setColor(Qt::white);
    light2->setIntensity(1.5f);

    sphereOne->addComponent(new Qt3D::QTransform(sphereOneTranslate));
    sphereOne->addComponent(sphereOneMaterial);
    sphereOne->addComponent(sphereMesh);
    sphereOne->addComponent(sceneLayer);
    sphereOne->addComponent(light2);

    Qt3D::QEntity *sphereTwo = new Qt3D::QEntity(rootEntity);
    Qt3D::QMaterial *sphereTwoMaterial = new Qt3D::QMaterial();
    sphereTwoMaterial->setEffect(sceneEffect);
    sphereTwoMaterial->addParameter(new Qt3D::QParameter(QStringLiteral("meshColor"), QColor(Qt::yellow)));

    Qt3D::QPointLight *light3 = new Qt3D::QPointLight();
    light3->setColor(Qt::blue);
    light3->setIntensity(0.5f);

    sphereTwo->addComponent(sphereMesh);
    sphereTwo->addComponent(sceneLayer);
    sphereTwo->addComponent(sphereTwoMaterial);
    sphereTwo->addComponent(light3);

    // Screen Quad
    Qt3D::QEntity *screenQuad = new Qt3D::QEntity(rootEntity);
    Qt3D::QMaterial *screenQuadMaterial = new Qt3D::QMaterial();
    screenQuadMaterial->addParameter(new Qt3D::QParameter(QStringLiteral("position"), gBuffer->positionTexture()));
    screenQuadMaterial->addParameter(new Qt3D::QParameter(QStringLiteral("normal"), gBuffer->normalTexture()));
    screenQuadMaterial->addParameter(new Qt3D::QParameter(QStringLiteral("color"), gBuffer->colorTexture()));
    screenQuadMaterial->addParameter(new Qt3D::QParameter(QStringLiteral("winSize"), QSize(1024, 1024)));
    PointLightBlock *lightsData = new PointLightBlock(screenQuadMaterial);
    lightsData->addLight(light1);
    lightsData->addLight(light2);
    lightsData->addLight(light3);

    finalEffect->gl3Technique()->addParameter(new Qt3D::QParameter(QStringLiteral("PointLightBlock"), QVariant::fromValue(lightsData)));
    finalEffect->gl2Technique()->addParameter(new Qt3D::QParameter(QStringLiteral("pointLights"), QVariant::fromValue(lightsData)));
    screenQuadMaterial->setEffect(finalEffect);

    Qt3D::QRotateTransform *screenPlaneRotation = new Qt3D::QRotateTransform();
    screenPlaneRotation->setAngleDeg(90);
    screenPlaneRotation->setAxis(QVector3D(1.0f, 0.0f, 0.0f));

    screenQuad->addComponent(quadLayer);
    screenQuad->addComponent(new Qt3D::QTransform(screenPlaneRotation));
    screenQuad->addComponent(screenQuadMaterial);
    screenQuad->addComponent(planeMesh);

    // Set root object of the scene
    engine.setRootEntity(rootEntity);
    // Show window
    view.show();

    return app.exec();
}
