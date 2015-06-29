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

#include <QGuiApplication>

#include <window.h>
#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcameralens.h>
#include <Qt3DCore/qaspectengine.h>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRenderer/qtorusmesh.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qsceneloader.h>

#include <Qt3DCore/qtranslatetransform.h>
#include <Qt3DCore/qmatrixtransform.h>
#include <Qt3DCore/qrotatetransform.h>
#include <Qt3DCore/qlookattransform.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DRenderer/qcameraselector.h>
#include <Qt3DRenderer/qrenderpassfilter.h>
#include <Qt3DRenderer/qtechniquefilter.h>
#include <Qt3DRenderer/qviewport.h>
#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qclearbuffer.h>

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
    // Torus
    Qt3D::QEntity *torusEntity = new Qt3D::QEntity(rootEntity);

    // Torus shape data
    Qt3D::QTorusMesh *torus = new Qt3D::QTorusMesh();
    torus->setRadius(40);
    torus->setMinorRadius(15);

    torusEntity->addComponent(torus);

    // TorusMesh Transform
    Qt3D::QTranslateTransform *torusTranslation = new Qt3D::QTranslateTransform();
    Qt3D::QRotateTransform *torusRotation = new Qt3D::QRotateTransform();
    Qt3D::QTransform *torusTransforms = new Qt3D::QTransform();

    torusTranslation->setTranslation(QVector3D(-5.0f, 3.5f, 2.0f));
    torusRotation->setAxis(QVector3D(1, 0, 0));
    torusRotation->setAngleDeg(35.0f);
    torusTransforms->addTransform(torusTranslation);
    torusTransforms->addTransform(torusRotation);
    torusEntity->addComponent(torusTransforms);

    // Scene file
    Qt3D::QEntity *sceneEntity = new Qt3D::QEntity(rootEntity);
    Qt3D::QSceneLoader  *scene = new Qt3D::QSceneLoader();
    scene->setObjectName(QStringLiteral("scene"));
    Qt3D::QTransform *sceneTransform = new Qt3D::QTransform();
    Qt3D::QTranslateTransform *sceneTranslateTransform = new Qt3D::QTranslateTransform();
    sceneTranslateTransform->setDx(2.5);
    sceneTranslateTransform->setDy(0.5);
    sceneTranslateTransform->setDz(-10);
    sceneTransform->addTransform(sceneTranslateTransform);
    sceneEntity->addComponent(sceneTransform);
//    scene->setSource(":/assets/gltf/wine/wine.json");
    scene->setSource(QUrl("qrc:/assets/test_scene.dae"));
    sceneEntity->addComponent(scene);

    // Camera
    Qt3D::QCamera *cameraEntity = new Qt3D::QCamera(rootEntity);
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));

    cameraEntity->lens()->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(-5, 0, -20.0f));
    cameraEntity->setViewCenter(QVector3D(11, 0, 5));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    input->setCamera(cameraEntity);

    // FrameGraph
    Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
    Qt3D::QTechniqueFilter *techniqueFilter = new Qt3D::QTechniqueFilter();
    Qt3D::QViewport *viewport = new Qt3D::QViewport(techniqueFilter);
    Qt3D::QClearBuffer *clearBuffer = new Qt3D::QClearBuffer(viewport);
    Qt3D::QCameraSelector *cameraSelector = new Qt3D::QCameraSelector(clearBuffer);
    (void) new Qt3D::QRenderPassFilter(cameraSelector);

    // TechiqueFilter and renderPassFilter are not implement yet
    viewport->setRect(QRectF(0, 0, 1, 1));
    clearBuffer->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);
    cameraSelector->setCamera(cameraEntity);
    frameGraph->setActiveFrameGraph(techniqueFilter);

    // Setting the FrameGraph
    rootEntity->addComponent(frameGraph);

    // Set root object of the scene
    engine.setRootEntity(rootEntity);
    // Show window
    view.show();

    return app.exec();
}
