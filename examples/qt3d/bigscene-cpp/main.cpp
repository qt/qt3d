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

#include <QPropertyAnimation>
#include <QUrl>
#include <QTimer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DRenderer/QParameter>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QCameraSelector>
#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/QForwardRenderer>
#include <qmath.h>

using namespace Qt3D;

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
    QEntity *root = new QEntity();

    // Camera
    QCamera *cameraEntity = new QCamera(root);
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, -250.0f, -50.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    input->setCamera(cameraEntity);

    // FrameGraph
    QFrameGraph *frameGraph = new QFrameGraph();
    QForwardRenderer *forwardRenderer = new QForwardRenderer();
    forwardRenderer->setCamera(cameraEntity);
    forwardRenderer->setClearColor(Qt::black);
    frameGraph->setActiveFrameGraph(forwardRenderer);
    root->addComponent(frameGraph);


    const float radius = 100.0f;
    const int max = 1000;
    const float det = 1.0f / max;
    // Scene
    for (int i = 0; i < max; i++) {
        QEntity *e = new QEntity();
        Qt3D::QTransform *transform = new Qt3D::QTransform();
        QTranslateTransform *translation = new QTranslateTransform();
        QRotateTransform *rotateX = new QRotateTransform();
        QRotateTransform *rotateZ = new QRotateTransform();
        QCylinderMesh *mesh = new QCylinderMesh();
        QPhongMaterial *material = new QPhongMaterial();
        mesh->setRings(50.0f);
        mesh->setSlices(30.0f);
        mesh->setRadius(2.5f);
        mesh->setLength(5.0f);

        const float angle = M_PI * 2.0f * i * det * 10.;

        material->setDiffuse(QColor(qFabs(qCos(angle)) * 255, 204, 75));
        material->setAmbient(Qt::gray);
        material->setSpecular(Qt::white);
        material->setShininess(150.0f);

        translation->setTranslation(QVector3D(radius * qCos(angle), 200.* i * det, radius * qSin(angle)));
        rotateX->setAxis(QVector3D(1.0f, 0.0f, 0.0f));
        rotateZ->setAxis(QVector3D(0.0f, 0.0f, 1.0f));
        rotateX->setAngleDeg(30.0f * i);
        rotateZ->setAngleDeg(45.0f * i);

        QPropertyAnimation *animX = new QPropertyAnimation(rotateX, QByteArrayLiteral("angle"));
        animX->setDuration(2400 * i);
        animX->setStartValue(QVariant::fromValue(i * 30.0f));
        animX->setEndValue(QVariant::fromValue((i + 1) * 390.0f));
        animX->setLoopCount(-1);
        animX->start();

        QPropertyAnimation *animZ = new QPropertyAnimation(rotateZ, QByteArrayLiteral("angle"));
        animZ->setDuration(2400 * i);
        animZ->setStartValue(QVariant::fromValue(i * 20.0f));
        animZ->setEndValue(QVariant::fromValue((i + 1) * 380.0f));
        animZ->setLoopCount(-1);
        animZ->start();

        transform->addTransform(rotateX);
        transform->addTransform(rotateZ);
        transform->addTransform(translation);
        e->addComponent(transform);
        e->addComponent(mesh);
        e->addComponent(material);
        e->setParent(root);
    }

    engine.setRootEntity(root);
    view.show();

    if (app.arguments().contains(("--bench")))
        QTimer::singleShot(25 * 1000, &app, &QCoreApplication::quit);

    return app.exec();
}
