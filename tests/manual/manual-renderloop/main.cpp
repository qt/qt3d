// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QGuiApplication>
#include <QTimer>
#include <QOpenGLContext>
#include <QWindow>
#include <qmath.h>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>


class ManualRenderer
{
public:
    ManualRenderer()
        : m_aspectEngine(new Qt3DCore::QAspectEngine())
        , m_renderAspect(new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Manual))
        , m_renderer(nullptr)
    {
    }

    ~ManualRenderer() {
        m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr());
        m_aspectEngine->unregisterAspect(m_renderAspect);
        delete m_renderAspect;
        delete m_aspectEngine;
    }

    void initialize(QWindow *window, QOpenGLContext *glCtx)
    {
        m_aspectEngine->registerAspect(m_renderAspect);
        m_aspectEngine->setRunMode(Qt3DCore::QAspectEngine::Manual);

        Qt3DRender::QRenderAspectPrivate *dRenderAspect = static_cast<decltype(dRenderAspect)>
                (Qt3DRender::QRenderAspectPrivate::get(m_renderAspect));
        m_renderer = dRenderAspect->m_renderer;
        m_renderer->setOpenGLContext(glCtx);
        m_renderer->initialize();

        m_rootEntity.reset(createSceneTree(window));
        m_aspectEngine->setRootEntity(m_rootEntity);
    }

    void render() {
        qDebug() << Q_FUNC_INFO << "Updating Scene";
        updateScene();
        qDebug() << Q_FUNC_INFO << "Processing Frame";
        // Launch jobs to process the frame
        m_aspectEngine->processFrame();
        qDebug() << Q_FUNC_INFO << "Rendering Frame";
        // Submit Render Queues
        m_renderer->render(true);
    }

private:
    Qt3DCore::QEntity *createSceneTree(QWindow *surface) {
        // Root entity in the 3D scene.
        Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;
        auto camera = new Qt3DRender::QCamera(rootEntity);
        camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
        camera->setPosition(QVector3D(0, 0, 40.0f));
        camera->setViewCenter(QVector3D(0, 0, 0));

        // Torus
        Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
        auto material = new Qt3DExtras::QPhongMaterial(rootEntity);
        auto torusTransform = new Qt3DCore::QTransform;
        auto torusMesh = new Qt3DExtras::QTorusMesh;

        torusMesh->setRadius(5);
        torusMesh->setMinorRadius(1);
        torusMesh->setRings(100);
        torusMesh->setSlices(20);

        torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
        torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

        torusEntity->addComponent(torusMesh);
        torusEntity->addComponent(material);
        torusEntity->addComponent(torusTransform);

        // FrameGraph
        auto forwardRenderer = new Qt3DExtras::QForwardRenderer();
        forwardRenderer->setSurface(surface);
        forwardRenderer->setCamera(camera);
        forwardRenderer->setClearColor(QColor(Qt::black));

        auto renderSettings = new Qt3DRender::QRenderSettings();
        renderSettings->setActiveFrameGraph(forwardRenderer);
        rootEntity->addComponent(renderSettings);

        return rootEntity;
    }

    void updateScene()
    {
        static int angle = 0;
        // Update camera position
        auto camera = m_rootEntity->findChild<Qt3DRender::QCamera *>();
        Q_ASSERT(camera);

        const double angleRad = qDegreesToRadians(double(angle));
        const QVector3D newPos(qSin(angleRad), 0.0, qCos(angleRad));
        camera->setPosition(newPos * 40.0f);

        qDebug() << Q_FUNC_INFO << "Camera Transform Matrix" << camera->transform()->matrix();
        qDebug() << Q_FUNC_INFO << "Camera ViewMatrix" << camera->viewMatrix();

        angle = (angle + 1) % 360;
    }

    Qt3DCore::QEntityPtr m_rootEntity;
    Qt3DCore::QAspectEngine *m_aspectEngine;
    Qt3DRender::QRenderAspect *m_renderAspect;
    Qt3DRender::Render::AbstractRenderer *m_renderer;
};

int main(int ac, char **av)
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#if QT_CONFIG(opengles2)
    format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
#endif
    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(ac, av);

    QWindow win;
    win.setSurfaceType(QSurface::OpenGLSurface);
    win.resize(1024, 768);
    win.setFormat(format);
    win.show();

    QOpenGLContext ctx;
    ctx.setFormat(format);
    const bool ctxCreated = ctx.create();

    if (!ctxCreated) {
        qWarning() << "Failed to create GL context";
        return -1;
    }

    ManualRenderer renderer;
    renderer.initialize(&win, &ctx);

    QTimer t;
    QObject::connect(&t, &QTimer::timeout, [&] { renderer.render(); });
    t.start(16);

    return app.exec();
}
