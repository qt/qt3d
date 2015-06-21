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
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QLookAtTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QForwardRenderer>
#include <Qt3DRenderer/QPhongMaterial>

#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QSphereMesh>
#include <Qt3DRenderer/QTorusMesh>

#include <QPropertyAnimation>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
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


    // Camera
    Qt3D::QCamera *cameraEntity = new Qt3D::QCamera(rootEntity);

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, -40.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    input->setCamera(cameraEntity);


    // FrameGraph
    Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
    Qt3D::QForwardRenderer *forwardRenderer = new Qt3D::QForwardRenderer();
    forwardRenderer->setClearColor(QColor::fromRgbF(0.0, 0.5, 1.0, 1.0));
    forwardRenderer->setCamera(cameraEntity);
    frameGraph->setActiveFrameGraph(forwardRenderer);

    // Material
    Qt3D::QMaterial *material = new Qt3D::QPhongMaterial(rootEntity);

    // Torus
    Qt3D::QEntity *torusEntity = new Qt3D::QEntity(rootEntity);
    Qt3D::QTorusMesh *torusMesh = new Qt3D::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    Qt3D::QTransform *torusTransform = new Qt3D::QTransform;
    Qt3D::QScaleTransform *torusScaleTransform = new Qt3D::QScaleTransform;
    torusScaleTransform->setScale3D(QVector3D(1.5, 1, 0.5));

    Qt3D::QRotateTransform *torusRotateTransform = new Qt3D::QRotateTransform;
    torusRotateTransform->setAxis(QVector3D(1, 0, 0));
    torusRotateTransform->setAngleDeg(45);

    torusTransform->addTransform(torusScaleTransform);
    torusTransform->addTransform(torusRotateTransform);


    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);


    // Sphere
    Qt3D::QEntity *sphereEntity = new Qt3D::QEntity(rootEntity);
    Qt3D::QSphereMesh *sphereMesh = new Qt3D::QSphereMesh;
    sphereMesh->setRadius(3);

    Qt3D::QTransform *sphereTransform = new Qt3D::QTransform;
    Qt3D::QTranslateTransform *sphereTranslateTransform = new Qt3D::QTranslateTransform;
    sphereTranslateTransform->setTranslation(QVector3D(20, 0, 0));

    Qt3D::QRotateTransform *sphereRotateTransform = new Qt3D::QRotateTransform;
    QPropertyAnimation *sphereRotateTransformAnimation = new QPropertyAnimation(sphereRotateTransform);
    sphereRotateTransformAnimation->setTargetObject(sphereRotateTransform);
    sphereRotateTransformAnimation->setPropertyName("angle");
    sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
    sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
    sphereRotateTransformAnimation->setDuration(10000);
    sphereRotateTransformAnimation->setLoopCount(-1);
    sphereRotateTransformAnimation->start();

    sphereRotateTransform->setAxis(QVector3D(0, 1, 0));
    sphereRotateTransform->setAngleDeg(0);

    sphereTransform->addTransform(sphereTranslateTransform);
    sphereTransform->addTransform(sphereRotateTransform);

    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereTransform);
    sphereEntity->addComponent(material);

    rootEntity->addComponent(frameGraph);

    engine.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
