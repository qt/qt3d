/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>

#include <exampleresources.h>

#include <Qt3DCore/window.h>
#include <Qt3DCore/camera.h>
#include <Qt3DCore/entity.h>
#include <Qt3DCore/cameralens.h>

#include <Qt3DRenderer/qtorusmesh.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DCore/qabstracteffect.h>
#include <Qt3DRenderer/texture.h>
#include <Qt3DRenderer/renderpass.h>
#include <Qt3DRenderer/scene.h>

#include <Qt3DCore/translatetransform.h>
#include <Qt3DCore/matrixtransform.h>
#include <Qt3DCore/rotatetransform.h>
#include <Qt3DCore/lookattransform.h>
#include <Qt3DCore/transform.h>

#include <Qt3DRenderer/cameraselector.h>
#include <Qt3DRenderer/renderpassfilter.h>
#include <Qt3DRenderer/techniquefilter.h>
#include <Qt3DRenderer/viewport.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DRenderer/framegraph.h>

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    initializeAssetResources("../exampleresources/example-assets.qrb");

    Qt3D::Window view;
    view.registerAspect(new Qt3D::RendererAspect());

    // Root entity
    Qt3D::Entity *rootEntity = new Qt3D::Entity();
    rootEntity->setObjectName(QStringLiteral("rootEntity"));
    // Torus
    Qt3D::Entity *torusEntity = new Qt3D::Entity();

    // Torus shape data
    Qt3D::QTorusMesh *torus = new Qt3D::QTorusMesh();
    torus->setRadius(40);
    torus->setMinorRadius(15);

    // Torus mesh holding the shape's data
    Qt3D::QMesh *torusMesh = new Qt3D::QMesh();
    torusMesh->setData(torus->data());
    torusEntity->addComponent(torusMesh);

    // TorusMesh Transform
    Qt3D::TranslateTransform *torusTranslation = new Qt3D::TranslateTransform();
    Qt3D::RotateTransform *torusRotation = new Qt3D::RotateTransform();
    Qt3D::Transform *torusTransforms = new Qt3D::Transform();

    torusTranslation->setTranslation(QVector3D(-5.0f, 3.5f, 2.0f));
    torusRotation->setAxis(QVector3D(1, 0, 0));
    torusRotation->setAngleDeg(35.0f);
    torusTransforms->appendTransform(torusTranslation);
    torusTransforms->appendTransform(torusRotation);
    torusEntity->addComponent(torusTransforms);

    // Scene file
    Qt3D::Scene  *scene = new Qt3D::Scene();
    scene->setObjectName(QStringLiteral("scene"));
    Qt3D::Transform *sceneTransform = new Qt3D::Transform();
    Qt3D::TranslateTransform *sceneTranslateTransform = new Qt3D::TranslateTransform();
    sceneTranslateTransform->setDx(2.5);
    sceneTranslateTransform->setDy(0.5);
    sceneTranslateTransform->setDz(-10);
    sceneTransform->appendTransform(sceneTranslateTransform);
    scene->addComponent(sceneTransform);
//    scene->setSource(":/assets/gltf/wine/wine.json");
    scene->setSource(":/assets/test_scene.dae");

    // Camera
    Qt3D::Camera *cameraEntity = new Qt3D::Camera();
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
    Qt3D::CameraLens *cameraLens = new Qt3D::CameraLens();
    Qt3D::Transform *cameraTransform = new Qt3D::Transform();
    Qt3D::LookAtTransform *cameraLookAtTransform = new Qt3D::LookAtTransform();

    cameraLens->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraLookAtTransform->setPosition(QVector3D(-5, 0, -20.0f));
    cameraLookAtTransform->setViewCenter(QVector3D(11, 0, 5));
    cameraLookAtTransform->setUpVector(QVector3D(0, 1, 0));
    cameraTransform->appendTransform(cameraLookAtTransform);
    cameraEntity->setTransform(cameraTransform);
    cameraEntity->setLens(cameraLens);
    view.setCamera(cameraEntity);

    // FrameGraph
    Qt3D::FrameGraph *frameGraph = new Qt3D::FrameGraph();
    Qt3D::TechniqueFilter *techniqueFilter = new Qt3D::TechniqueFilter();
    Qt3D::CameraSelector *cameraSelector = new Qt3D::CameraSelector();
    Qt3D::RenderPassFilter *defaultRenderPassFilter = new Qt3D::RenderPassFilter();
    Qt3D::Viewport *viewport = new Qt3D::Viewport();

    // TechiqueFilter and renderPassFilter are not implement yet

    viewport->setRect(QRectF(0, 0, 1, 1));
    viewport->addChild(cameraSelector);
    cameraSelector->setCamera(cameraEntity);
    cameraSelector->addChild(defaultRenderPassFilter);
    techniqueFilter->addChild(viewport);
    frameGraph->setActiveFrameGraph(techniqueFilter);

    // Setting the FrameGraph
    rootEntity->addComponent(frameGraph);

    // Build Node Tree
    rootEntity->addChild(cameraEntity);
    rootEntity->addChild(torusEntity);
    rootEntity->addChild(scene);

    // Set root object of the scene
    view.setRootObject(rootEntity);
    // Show window
    view.show();

    return app.exec();
}
