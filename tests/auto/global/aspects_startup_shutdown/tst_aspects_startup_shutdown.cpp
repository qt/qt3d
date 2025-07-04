// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/QInputSettings>

#include <Qt3DLogic/QLogicAspect>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QtGui/qquaternion.h>

#include <QPropertyAnimation>
#include <QThread>

namespace {

Qt3DCore::QEntity *createScene(QWindow *w)
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    // Camera
    Qt3DRender::QCamera *camera = new Qt3DRender::QCamera(rootEntity);
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // FrameGraph
    Qt3DRender::QRenderSettings *renderSettings = new Qt3DRender::QRenderSettings();
    Qt3DExtras::QForwardRenderer *forwardRenderer = new Qt3DExtras::QForwardRenderer();
    forwardRenderer->setSurface(w);
    forwardRenderer->setCamera(camera);
    forwardRenderer->setClearColor(QColor(Qt::blue));
    renderSettings->setActiveFrameGraph(forwardRenderer);
    rootEntity->addComponent(renderSettings);

    // InputSettings
    Qt3DInput::QInputSettings *inputSettigns = new Qt3DInput::QInputSettings();
    rootEntity->addComponent(inputSettigns);

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);

    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform;
    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);

    // Sphere
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(3);

    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;

    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereTransform);
    sphereEntity->addComponent(material);

    return rootEntity;
}

} // anonymous

class tst_AspectsStartupShutdown : public QObject
{
    Q_OBJECT

public:

private slots:

    void checkStartupAndShutdownImmediately()
    {
        QSKIP("Fails on CI for some unexplained reason");

        // GIVEN
        QWindow *win = new QWindow();
        win->setSurfaceType(QSurface::OpenGLSurface);
        win->resize(1024, 768);
        win->show();

        // WHEN
        Qt3DCore::QAspectEngine *engine = new Qt3DCore::QAspectEngine();
        engine->registerAspect(new Qt3DRender::QRenderAspect());
        engine->registerAspect(new Qt3DInput::QInputAspect());
        engine->registerAspect(new Qt3DLogic::QLogicAspect());
        QPointer<Qt3DCore::QEntity> scene = createScene(win);
        engine->setRootEntity(Qt3DCore::QEntityPtr(scene.data()));

        // THEN
        QCOMPARE(engine->rootEntity().data(), scene.data());

        // WHEN
        win->close();
        delete engine;

        // THEN -> shouldn't crash or deadlock
        delete win;
    }

    void checkStartupAndShutdownAfterAFewFrames()
    {
        QSKIP("Fails on CI for some unexplained reason");

        // GIVEN
        QWindow *win = new QWindow();
        win->setSurfaceType(QSurface::OpenGLSurface);
        win->resize(1024, 768);
        win->show();

        // WHEN
        Qt3DCore::QAspectEngine *engine = new Qt3DCore::QAspectEngine();
        engine->registerAspect(new Qt3DRender::QRenderAspect());
        engine->registerAspect(new Qt3DInput::QInputAspect());
        engine->registerAspect(new Qt3DLogic::QLogicAspect());
        QPointer<Qt3DCore::QEntity> scene = createScene(win);
        engine->setRootEntity(Qt3DCore::QEntityPtr(scene.data()));

        // THEN
        QCOMPARE(engine->rootEntity().data(), scene.data());

        // Allow a few frames
        for (int i = 0; i < 10; ++i) {
            QCoreApplication::processEvents();
            QThread::msleep(16);
        }

        // WHEN
        // Right now we need to close the window before destroying
        // the engine to prevent the render thread from trying to render
        // while things are being destroyed
        win->close();
        delete engine;

        // THEN -> shouldn't crash or deadlock
        delete win;
    }
};

QTEST_MAIN(tst_AspectsStartupShutdown)

#include "tst_aspects_startup_shutdown.moc"
