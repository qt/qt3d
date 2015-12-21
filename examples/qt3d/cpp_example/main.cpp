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
#include <Qt3DCore/qtransform.h>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/qtorusmesh.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/qframegraph.h>
#include <Qt3DRender/qclearbuffer.h>

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    Window view;
    Qt3DCore::QAspectEngine engine;
    engine.registerAspect(new Qt3DRender::QRenderAspect());
    Qt3DInput::QInputAspect *input = new Qt3DInput::QInputAspect;
    engine.registerAspect(input);
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.setData(data);

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    rootEntity->setObjectName(QStringLiteral("rootEntity"));
    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);

    // Torus shape data
    Qt3DRender::QTorusMesh *torus = new Qt3DRender::QTorusMesh();
    torus->setRadius(40);
    torus->setMinorRadius(15);

    torusEntity->addComponent(torus);

    // TorusMesh Transform
    Qt3DCore::QTransform *torusTransforms = new Qt3DCore::QTransform();
    torusTransforms->setTranslation(QVector3D(-5.0f, 3.5f, 2.0f));
    torusTransforms->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 35.0f));
    torusEntity->addComponent(torusTransforms);

    // Scene file
    Qt3DCore::QEntity *sceneEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QSceneLoader *scene = new Qt3DRender::QSceneLoader();
    scene->setObjectName(QStringLiteral("scene"));
    Qt3DCore::QTransform *sceneTransform = new Qt3DCore::QTransform();
    sceneTransform->setTranslation(QVector3D(2.5f, 0.5f, -10.0f));
    sceneEntity->addComponent(sceneTransform);
    scene->setSource(QUrl("qrc:/assets/test_scene.dae"));
    sceneEntity->addComponent(scene);

    // Camera
    Qt3DCore::QCamera *cameraEntity = new Qt3DCore::QCamera(rootEntity);
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
    cameraEntity->lens()->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(-5, 0, -20.0f));
    cameraEntity->setViewCenter(QVector3D(11, 0, 5));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    input->setCamera(cameraEntity);

    // FrameGraph
    Qt3DRender::QFrameGraph *frameGraph = new Qt3DRender::QFrameGraph();
    Qt3DRender::QTechniqueFilter *techniqueFilter = new Qt3DRender::QTechniqueFilter();
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport(techniqueFilter);
    Qt3DRender::QClearBuffer *clearBuffer = new Qt3DRender::QClearBuffer(viewport);
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(clearBuffer);
    (void) new Qt3DRender::QRenderPassFilter(cameraSelector);

    // TechiqueFilter and renderPassFilter are not implement yet
    viewport->setRect(QRectF(0, 0, 1, 1));
    clearBuffer->setBuffers(Qt3DRender::QClearBuffer::ColorDepthBuffer);
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
