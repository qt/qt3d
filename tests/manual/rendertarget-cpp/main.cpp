// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QEffect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QCameraSelector>
#include <QCuboidMesh>
#include <QDebugOverlay>
#include <QFilterKey>
#include <QFrustumCulling>
#include <QPropertyAnimation>
#include <QRenderSurfaceSelector>
#include <QRenderSurfaceSelector>
#include <QTechniqueFilter>
#include <QTexture>
#include <QViewport>

#include "qt3dwindow.h"
#include "orbittransformcontroller.h"
#include "qorbitcameracontroller.h"

class Renderer : public Qt3DRender::QRenderSurfaceSelector
{
public:
    Renderer()
        : Qt3DRender::QRenderSurfaceSelector()
        , m_texture(new Qt3DRender::QTexture2D())
        , m_renderTarget(new Qt3DRender::QRenderTarget())
        , m_renderTargetOutput(new Qt3DRender::QRenderTargetOutput())
        , m_renderTargetSelector(new Qt3DRender::QRenderTargetSelector())
        , m_fboPassFilter(new Qt3DRender::QRenderPassFilter())
        , m_childCameraSelector(new Qt3DRender::QCameraSelector())
        , m_childClearBuffer(new Qt3DRender::QClearBuffers())
        , m_childFrustumCulling(new Qt3DRender::QFrustumCulling())

        , m_technique{new Qt3DRender::QTechniqueFilter{}}
        , m_viewport(new Qt3DRender::QViewport())
        , m_mainCameraSelector(new Qt3DRender::QCameraSelector())
        , m_mainClearBuffer(new Qt3DRender::QClearBuffers())
        , m_mainFrustumCulling(new Qt3DRender::QFrustumCulling())
        , m_debugOverlay(new Qt3DRender::QDebugOverlay())
    {
        // First set-up the framegraph branch that will render to texture
        m_texture->setWidth(512);
        m_texture->setHeight(512);
        m_texture->setFormat(Qt3DRender::QAbstractTexture::RGBA8_UNorm);
        m_texture->setParent(m_renderTargetOutput);

        m_renderTargetOutput->setTexture(m_texture);
        m_renderTargetOutput->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);
        m_renderTargetOutput->setParent(m_renderTarget);

        m_renderTarget->addOutput(m_renderTargetOutput);
        m_renderTarget->setParent(m_renderTargetSelector);

        m_childFrustumCulling->setParent(m_childClearBuffer);
        m_childClearBuffer->setParent(m_childCameraSelector);
        m_childCameraSelector->setParent(m_renderTargetSelector);

        m_renderTargetSelector->setTarget(m_renderTarget);
        m_renderTargetSelector->setParent(m_fboPassFilter);
        m_fboPassFilter->setParent(this);
        {
            auto fboPass = new Qt3DRender::QFilterKey(this);
            fboPass->setName(QStringLiteral("pass"));
            fboPass->setValue(QStringLiteral("fbo"));
            m_fboPassFilter->addMatch(fboPass);
        }


        // Then the framegraph branch that renders the texture on a sphere and that sphere to the screen
        m_debugOverlay->setParent(m_mainFrustumCulling);
        m_debugOverlay->setEnabled(false);
        m_mainFrustumCulling->setParent(m_mainClearBuffer);
        m_mainClearBuffer->setParent(m_mainCameraSelector);
        m_mainCameraSelector->setParent(m_viewport);
        m_viewport->setParent(m_technique);
        m_technique->setParent(this);

        m_viewport->setNormalizedRect(QRectF(0.0, 0.0, 1.0, 1.0));
        m_mainClearBuffer->setClearColor(Qt::white);
        m_mainClearBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
        m_childClearBuffer->setClearColor(Qt::blue);
        m_childClearBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

        {
            auto forwardRenderingStyle = new Qt3DRender::QFilterKey(this);
            forwardRenderingStyle->setName(QStringLiteral("renderingStyle"));
            forwardRenderingStyle->setValue(QStringLiteral("forward"));
            m_technique->addMatch(forwardRenderingStyle);
        }
    }

    void setCamera(Qt3DCore::QEntity *camera)
    {
        m_childCameraSelector->setCamera(camera);
        m_mainCameraSelector->setCamera(camera);
    }
    Qt3DRender::QTexture2D* texture() const noexcept { return m_texture; }

private:
    Qt3DRender::QTexture2D* m_texture{};
    Qt3DRender::QRenderTarget* m_renderTarget{};
    Qt3DRender::QRenderTargetOutput* m_renderTargetOutput{};
    Qt3DRender::QRenderTargetSelector* m_renderTargetSelector{};
    Qt3DRender::QRenderPassFilter* m_fboPassFilter{};
    Qt3DRender::QCameraSelector *m_childCameraSelector{};
    Qt3DRender::QClearBuffers *m_childClearBuffer{};
    Qt3DRender::QFrustumCulling *m_childFrustumCulling{};

    Qt3DRender::QTechniqueFilter* m_technique{};
    Qt3DRender::QViewport *m_viewport{};
    Qt3DRender::QCameraSelector *m_mainCameraSelector{};
    Qt3DRender::QClearBuffers *m_mainClearBuffer{};
    Qt3DRender::QFrustumCulling *m_mainFrustumCulling{};

    Qt3DRender::QDebugOverlay *m_debugOverlay{};
};

Qt3DCore::QEntity *createScene(const Renderer& renderer)
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    {
        // Material
        Qt3DRender::QMaterial *childMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);

        // Put this in the fbo render pass
        {
            auto fboPass = new Qt3DRender::QFilterKey(childMaterial);
            fboPass->setName(QStringLiteral("pass"));
            fboPass->setValue(QStringLiteral("fbo"));
            for (auto technique : childMaterial->effect()->techniques())
            {
                const auto keys = technique->filterKeys();
                for (auto key : keys)
                    technique->removeFilterKey(key);

                for (auto pass : technique->renderPasses())
                    pass->addFilterKey(fboPass);
            }
        }

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
        torusEntity->addComponent(childMaterial);

        // Sphere
        Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
        sphereMesh->setRadius(3);
        sphereMesh->setGenerateTangents(true);

        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;
        OrbitTransformController *controller = new OrbitTransformController(sphereTransform);
        controller->setTarget(sphereTransform);
        controller->setRadius(20.0f);

        QPropertyAnimation *sphereRotateTransformAnimation = new QPropertyAnimation(sphereTransform);
        sphereRotateTransformAnimation->setTargetObject(controller);
        sphereRotateTransformAnimation->setPropertyName("angle");
        sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
        sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
        sphereRotateTransformAnimation->setDuration(10000);
        sphereRotateTransformAnimation->setLoopCount(-1);
        sphereRotateTransformAnimation->start();

        sphereEntity->addComponent(sphereMesh);
        sphereEntity->addComponent(sphereTransform);
        sphereEntity->addComponent(childMaterial);
    }
    {
        auto mainMaterial = new Qt3DExtras::QTextureMaterial(rootEntity);
        mainMaterial->setTexture(renderer.texture());

        // Sphere
        auto sphereEntity = new Qt3DCore::QEntity(rootEntity);
        auto cubeMesh = new Qt3DExtras::QCuboidMesh;
        auto sphereTransform = new Qt3DCore::QTransform;
        sphereTransform->setRotationY(100);

        cubeMesh->setXExtent(10);
        cubeMesh->setYExtent(10);
        cubeMesh->setZExtent(10);

        sphereEntity->addComponent(cubeMesh);
        sphereEntity->addComponent(sphereTransform);
        sphereEntity->addComponent(mainMaterial);
    }

    return rootEntity;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    auto renderer = new Renderer;
    view.setActiveFrameGraph(renderer);

    Qt3DCore::QEntity *scene = createScene(*renderer);

    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    renderer->setCamera(camera);
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    view.setRootEntity(scene);
    view.show();

    return app.exec();
}
