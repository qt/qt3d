// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "entity.h"

#include <QGuiApplication>

#include <QPropertyAnimation>
#include <QUrl>
#include <QTimer>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DRender/QParameter>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <qmath.h>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QDebugOverlay>

using namespace Qt3DCore;
using namespace Qt3DRender;

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);
    Qt3DExtras::Qt3DWindow view;

    // FrameGraph
    {
        QRenderSurfaceSelector *surfaceSelector = new QRenderSurfaceSelector();
        QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(surfaceSelector);
        cameraSelector->setCamera(view.camera());
        QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(cameraSelector);
        clearBuffers->setClearColor(QColor(Qt::gray));
        clearBuffers->setBuffers(QClearBuffers::ColorDepthBuffer);
        new QNoDraw(clearBuffers);

        const QRectF viewports[] = {
            {0.0f, 0.0f, 0.25f, 0.25f},
            {0.25f, 0.0f, 0.25f, 0.25f},
            {0.5f, 0.0f, 0.25f, 0.25f},
            {0.75f, 0.0f, 0.25f, 0.25f},
            {0.0f, 0.25f, 0.25f, 0.25f},
            {0.25f, 0.25f, 0.25f, 0.25f},
            {0.5f, 0.25f, 0.25f, 0.25f},
            {0.75f, 0.25f, 0.25f, 0.25f},
            {0.0f, 0.5f, 0.25f, 0.25f},
            {0.25f, 0.5f, 0.25f, 0.25f},
            {0.5f, 0.5f, 0.25f, 0.25f},
            {0.75f, 0.5f, 0.25f, 0.25f},
            {0.0f, 0.75f, 0.25f, 0.25f},
            {0.25f, 0.75f, 0.25f, 0.25f},
            {0.5f, 0.75f, 0.25f, 0.25f},
            {0.75f, 0.75f, 0.25f, 0.25f},
        };

        for (const QRectF &vp : viewports) {
            QViewport *viewport = new Qt3DRender::QViewport(cameraSelector);
            viewport->setNormalizedRect(vp);
        }

        new QDebugOverlay(qobject_cast<Qt3DCore::QNode *>(cameraSelector->children().last()));

        view.setActiveFrameGraph(surfaceSelector);
    }


    QEntity *root = new QEntity();

    // Mesh
    auto *mesh = new Qt3DExtras::QCylinderMesh(root);
    mesh->setRings(50.0f);
    mesh->setSlices(30.0f);
    mesh->setRadius(2.5f);
    mesh->setLength(5.0f);

    // Material
    auto phongMaterial = new Qt3DExtras::QPhongMaterial(root);
    auto effect = phongMaterial->effect();

    // Camera
    QCamera *cameraEntity = view.camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 250.0f, 50.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QFirstPersonCameraController *camController = new Qt3DExtras::QFirstPersonCameraController(root);
    camController->setCamera(cameraEntity);

    const double radius = 100.0;
    const int max = 1000;
    const double det = 1.0 / max;

    // Scene
    for (int i = 0; i < max; i++) {
        Entity *e = new Entity(effect, root);
        e->addComponent(mesh);
        const double angle = M_PI * 2.0 * double(i) * det * 10.;

        e->setDiffuseColor(QColor(int(qFabs(qCos(angle)) * 255.0), 204, 75));
        e->setPosition(QVector3D(float(radius * qCos(angle)),
                                 float(-200.0 * i * det),
                                 float(radius * qSin(angle))));
        e->setTheta(30.0f * i);
        e->setPhi(45.0f * i);

        QPropertyAnimation *animX = new QPropertyAnimation(e, QByteArrayLiteral("theta"));
        animX->setDuration(2400 * (i + 1));
        animX->setStartValue(QVariant::fromValue(i * 30.0f));
        animX->setEndValue(QVariant::fromValue((i + 1) * 390.0f));
        animX->setLoopCount(-1);
        animX->start();

        QPropertyAnimation *animZ = new QPropertyAnimation(e, QByteArrayLiteral("phi"));
        animZ->setDuration(2400 * (i + 1));
        animZ->setStartValue(QVariant::fromValue(i * 20.0f));
        animZ->setEndValue(QVariant::fromValue((i + 1) * 380.0f));
        animZ->setLoopCount(-1);
        animZ->start();
    }

    view.setRootEntity(root);
    view.show();

    if (app.arguments().contains(("--bench")))
        QTimer::singleShot(25 * 1000, &app, &QCoreApplication::quit);

    return app.exec();
}
