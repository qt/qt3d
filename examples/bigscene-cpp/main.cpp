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
#include <QPropertyAnimation>
#include <Qt3DCore/Window>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DRenderer/QViewport>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QClearBuffer>
#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QCameraSelector>
#include <Qt3DRenderer/QTechniqueFilter>
#include <qmath.h>

using namespace Qt3D;

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    Window view;
    view.registerAspect(new QRenderAspect());

    QEntity *root = new QEntity();

    // Camera
    QCamera *cameraEntity = new QCamera(root);
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, -20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    view.setCamera(cameraEntity);

    // FrameGraph
    QFrameGraph *frameGraph = new QFrameGraph();
    QTechniqueFilter *techniqueFilter = new QTechniqueFilter();
    QViewport *viewport = new QViewport(techniqueFilter);
    QClearBuffer *clearBuffer = new QClearBuffer(viewport);
    QCameraSelector *cameraSelector = new QCameraSelector(clearBuffer);
    viewport->setRect(QRectF(0, 0, 1, 1));
    clearBuffer->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);
    cameraSelector->setCamera(cameraEntity);
    frameGraph->setActiveFrameGraph(techniqueFilter);
    root->addComponent(frameGraph);

    float radius = 300.0f;
    int max = 1000;
    // Scene
    for (int i = 0; i < max; i++) {
        QEntity *e = new QEntity();
        Qt3D::QTransform *transform = new Qt3D::QTransform();
        QTranslateTransform *translation = new QTranslateTransform();
        QRotateTransform *rotateX = new QRotateTransform();
        QRotateTransform *rotateZ = new QRotateTransform();
        QCylinderMesh *mesh = new QCylinderMesh();

        mesh->setRings(50.0f);
        mesh->setSlices(30.0f);
        mesh->setRadius(5.0f);
        mesh->setLength(10.0f);

        float det = 1.0f / max;
        float angle = M_PI * 2.0f * i * det;

        translation->setTranslation(QVector3D(radius * cos(angle), 20.0f * cos(i * 0.15f * M_PI * 2.0f), radius * sin(angle)));
        rotateX->setAxis(QVector3D(1.0f, 0.0f, 0.0f));
        rotateZ->setAxis(QVector3D(0.0f, 0.0f, 1.0f));
        rotateX->setAngleDeg(30.0f * i);
        rotateZ->setAngleDeg(45.0f * i);

        QPropertyAnimation *animX = new QPropertyAnimation(rotateX, "angle");
        animX->setDuration(1200);
        animX->setStartValue(QVariant::fromValue(i * det * 30.0f));
        animX->setEndValue(QVariant::fromValue((i + 1) * det * 390.0f));
        animX->setLoopCount(-1);
        animX->start();

        QPropertyAnimation *animZ = new QPropertyAnimation(rotateZ, "angle");
        animZ->setDuration(1700);
        animZ->setStartValue(QVariant::fromValue(i * det * 20.0f));
        animZ->setEndValue(QVariant::fromValue((i + 1) * det * 380.0f));
        animZ->setLoopCount(-1);
        animZ->start();

        transform->addTransform(rotateX);
        transform->addTransform(rotateZ);
        transform->addTransform(translation);
        e->addComponent(transform);
        e->addComponent(mesh);
        e->setParent(root);
    }

    view.setRootEntity(root);
    view.show();
    return app.exec();
}
