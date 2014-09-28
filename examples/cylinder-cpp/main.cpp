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

#include <Qt3DCore/window.h>
#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcameralens.h>

#include <Qt3DRenderer/qcylindermesh.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DCore/qabstracteffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qrenderpass.h>

#include <Qt3DCore/qscaletransform.h>
#include <Qt3DCore/qrotatetransform.h>
#include <Qt3DCore/qlookattransform.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DRenderer/qcameraselector.h>
#include <Qt3DRenderer/qrenderpassfilter.h>
#include <Qt3DRenderer/qtechniquefilter.h>
#include <Qt3DRenderer/qviewport.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qclearbuffer.h>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Qt3D::Window view;
    view.registerAspect(new Qt3D::RendererAspect());

    // Root entity
    Qt3D::QEntity *rootEntity = new Qt3D::QEntity();

    // Camera
    Qt3D::QCamera *cameraEntity = new Qt3D::QCamera();
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
    Qt3D::QCameraLens *cameraLens = new Qt3D::QCameraLens();
    Qt3D::QTransform *cameraTransform = new Qt3D::QTransform();
    Qt3D::QLookAtTransform *cameraLookAtTransform = new Qt3D::QLookAtTransform();

    cameraLens->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraLookAtTransform->setPosition(QVector3D(0, 0, -20.0f));
    cameraLookAtTransform->setUpVector(QVector3D(0, 1, 0));
    cameraLookAtTransform->setViewCenter(QVector3D(0, 0, 0));
    cameraTransform->appendTransform(cameraLookAtTransform);
    cameraEntity->setTransform(cameraTransform);
    cameraEntity->setLens(cameraLens);
    view.setCamera(cameraEntity);

    // FrameGraph
    Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
    Qt3D::QTechniqueFilter *techniqueFilter = new Qt3D::QTechniqueFilter();
    Qt3D::QCameraSelector *cameraSelector = new Qt3D::QCameraSelector();
    Qt3D::QRenderPassFilter *defaultRenderPassFilter = new Qt3D::QRenderPassFilter();
    Qt3D::QViewport *viewport = new Qt3D::QViewport();
    Qt3D::QClearBuffer *clearBuffer = new Qt3D::QClearBuffer();

    // TechiqueFilter and renderPassFilter are not implement yet

    viewport->setRect(QRectF(0, 0, 1, 1));
    viewport->addChild(clearBuffer);
    clearBuffer->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);
    clearBuffer->addChild(cameraSelector);
    cameraSelector->setCamera(cameraEntity);
    cameraSelector->addChild(defaultRenderPassFilter);
    techniqueFilter->addChild(viewport);

    frameGraph->setActiveFrameGraph(techniqueFilter);


    // Cylinder shape data
    Qt3D::QCylinderMesh *cylinder = new Qt3D::QCylinderMesh();
    cylinder->setRadius(1);
    cylinder->setLength(3);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    // CylinderMesh Transform
    Qt3D::QScaleTransform *cylinderScale = new Qt3D::QScaleTransform();
    Qt3D::QRotateTransform *cylinderRotation = new Qt3D::QRotateTransform();
    Qt3D::QTransform *cylinderTransforms = new Qt3D::QTransform();

    cylinderScale->setScale3D(QVector3D(1.5f, 1.5f, 1.5f));
    cylinderRotation->setAngleDeg(45.0f);
    cylinderRotation->setAxis(QVector3D(1, 0, 0));

    cylinderTransforms->appendTransform(cylinderScale);
    cylinderTransforms->appendTransform(cylinderRotation);

    // Cylinder
    Qt3D::QEntity *cylinderEntity = new Qt3D::QEntity();
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(cylinderTransforms);

    // Setting the FrameGraph
    rootEntity->addComponent(frameGraph);

    // Build Node Tree
    rootEntity->addChild(cameraEntity);
    rootEntity->addChild(cylinderEntity);

    // Set root object of the scene
    view.setRootObject(rootEntity);
    // Show window
    view.show();

    return app.exec();
}
