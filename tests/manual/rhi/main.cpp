/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPerVertexColorMaterial>

#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>

#include <QPropertyAnimation>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qorbitcameracontroller.h>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QNoDraw>
#include <QColor>
#include <QVector2D>
#include <QUrl>
#include <QTimer>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QTexture>
#include <qmath.h>

static const constexpr auto vertex_shader = R"_(#version 450

/**
 * Unicode comment: Ѧ𝙱ƇᗞΣ
 */

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 0) out vec3 color;

layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
    mat4 inverseViewMatrix;
    mat4 inverseProjectionMatrix;
    mat4 inverseViewProjectionMatrix;
    mat4 viewportMatrix;
    mat4 inverseViewportMatrix;
    vec4 textureTransformMatrix;
    vec3 eyePosition;
    float aspectRatio;
    float gamma;
    float exposure;
    float time;
};
layout(std140, binding = 1) uniform qt3d_command_uniforms {
    mat4 modelMatrix;
    mat4 inverseModelMatrix;
    mat4 modelViewMatrix;
    mat3 modelNormalMatrix;
    mat4 inverseModelViewMatrix;
    mat4 mvp;
    mat4 inverseModelViewProjectionMatrix;
};
void main()
{
    color = vertexColor;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)_";

static const constexpr auto fragment_shader = R"_(#version 450

/**
 * Unicode comment: Ѧ𝙱ƇᗞΣ
 */

layout(location = 0) out vec4 fragColor;
layout(location = 0) in vec3 color;

layout(std140, binding = 0) uniform qt3d_render_view_uniforms {
  mat4 viewMatrix;
  mat4 projectionMatrix;
  mat4 viewProjectionMatrix;
  mat4 inverseViewMatrix;
  mat4 inverseProjectionMatrix;
  mat4 inverseViewProjectionMatrix;
  mat4 viewportMatrix;
  mat4 inverseViewportMatrix;
  vec4 textureTransformMatrix;
  vec3 eyePosition;
  float aspectRatio;
  float gamma;
  float exposure;
  float time;
};
layout(std140, binding = 1) uniform qt3d_command_uniforms {
  mat4 modelMatrix;
  mat4 inverseModelMatrix;
  mat4 modelViewMatrix;
  mat3 modelNormalMatrix;
  mat4 inverseModelViewMatrix;
  mat4 mvp;
  mat4 inverseModelViewProjectionMatrix;
};
layout(std140, binding = 2) uniform custom_ubo {
   vec3 colorFactor;
};

layout(binding = 3) uniform sampler2D myTexture;
void main()
{
    vec2 texCoord = color.xz;
    vec2 rhiTexCoord = textureTransformMatrix.xy * texCoord+ textureTransformMatrix.zw;
    fragColor =  vec4(color * colorFactor, 1.0);

    fragColor *= texture(myTexture, rhiTexCoord);
}

)_";

class Material : public Qt3DRender::QMaterial
{
public:
    explicit Material(Qt3DCore::QNode *parent = nullptr)
    : QMaterial(parent)
    , m_effect(new Qt3DRender::QEffect(this))
{
    setEffect(m_effect);

    m_testParam = new Qt3DRender::QParameter(QStringLiteral("example"), float(0.5));

    m_effect->addParameter(m_testParam);

    m_filter = new Qt3DRender::QFilterKey(this);
    m_filter->setName(QStringLiteral("renderingStyle"));
    m_filter->setValue(QStringLiteral("forward"));

    m_technique = new Qt3DRender::QTechnique(m_effect);
    m_technique->addFilterKey(m_filter);

    m_effect->addTechnique(m_technique);

    m_program = new Qt3DRender::QShaderProgram(m_effect);
    m_program->setVertexShaderCode(vertex_shader);
    m_program->setFragmentShaderCode(fragment_shader);

    m_renderPass = new Qt3DRender::QRenderPass(m_effect);

    m_renderPass->setShaderProgram(m_program);

    m_technique->addRenderPass(m_renderPass);

    m_technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::RHI);
}
private:
    Qt3DRender::QEffect *m_effect{};
    Qt3DRender::QParameter *m_testParam{};
    Qt3DRender::QFilterKey *m_filter{};
    Qt3DRender::QTechnique *m_technique{};
    Qt3DRender::QShaderProgram *m_program{};
    Qt3DRender::QRenderPass *m_renderPass{};
};

int main(int argc, char* argv[])
{
    qputenv("QT3D_RENDERER", "rhi");
    QGuiApplication app(argc, argv);

    auto api = Qt3DRender::API::OpenGL;
    if (argc >= 2) {

#ifdef Q_OS_WIN
        if (argv[1] == QByteArrayLiteral("--d3d11")) api = Qt3DRender::API::DirectX;
#endif

#if QT_CONFIG(vulkan)
        if (argv[1] == QByteArrayLiteral("--vulkan")) api = Qt3DRender::API::Vulkan;
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_IOS)
        if (argv[1] == QByteArrayLiteral("--metal")) api = Qt3DRender::API::Metal;
#endif
    }

    Qt3DExtras::Qt3DWindow view{nullptr, api};

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *topViewCamera = new Qt3DRender::QCamera(rootEntity);
    topViewCamera->setPosition(QVector3D(0, 40, 0));
    topViewCamera->setViewCenter(QVector3D(0, 0, 0));
    topViewCamera->setUpVector(QVector3D(0, 0, 1));
    topViewCamera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);

    Qt3DRender::QCamera *cameraEntity = view.camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 40.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    {
        // Custom FG
        auto *surfaceSelector = new Qt3DRender::QRenderSurfaceSelector();
        surfaceSelector->setSurface(&view);

        // RV 1
        auto *clearBuffer = new Qt3DRender::QClearBuffers(surfaceSelector);
        clearBuffer->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
        clearBuffer->setClearColor(QColor::fromRgbF(0.1, 0.5, 0.0, 1.0));
        auto *noDraw = new Qt3DRender::QNoDraw(clearBuffer);

        // RV 2
        auto *cameraSelector1 = new Qt3DRender::QCameraSelector(surfaceSelector);
        cameraSelector1->setCamera(view.camera());
        auto *viewport1 = new Qt3DRender::QViewport(cameraSelector1);
        viewport1->setNormalizedRect(QRectF(0.0f, 0.0f, 0.5f, 0.5f));

        // RV3
        auto *cameraSelector2 = new Qt3DRender::QCameraSelector(surfaceSelector);
        cameraSelector2->setCamera(topViewCamera);
        auto *viewport2 = new Qt3DRender::QViewport(cameraSelector2);
        viewport2->setNormalizedRect(QRectF(0.5f, 0.5f, 0.5f, 0.5f));

        view.setActiveFrameGraph(surfaceSelector);
    }

    QTimer *cameraAnimationTimer = new QTimer(&view);
    QObject::connect(cameraAnimationTimer, &QTimer::timeout,
                     [cameraEntity] {
        static int angle = 0;
        const float radius = 40.0f;
        const float anglef = qDegreesToRadians(float(angle));
        cameraEntity->setPosition(QVector3D(qSin(anglef), 0.0f, qCos(anglef))  * radius);
        angle += 1;
    });
    cameraAnimationTimer->start(16);

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(cameraEntity);

    // Material
    Qt3DRender::QMaterial *material = new Material(rootEntity);
    Qt3DRender::QParameter *parameter = new Qt3DRender::QParameter(QStringLiteral("colorFactor"), QColor(Qt::white));
    material->addParameter(parameter);

    Qt3DRender::QTextureLoader *textureLoader = new Qt3DRender::QTextureLoader{};
    textureLoader->setSource(QUrl{QStringLiteral("qrc:///qtlogo.png")});
    Qt3DRender::QParameter *texture = new Qt3DRender::QParameter(QStringLiteral("myTexture"), textureLoader);
    material->addParameter(texture);

    QTimer *parameterAnimationTimer = new QTimer(&view);
    QObject::connect(parameterAnimationTimer, &QTimer::timeout,
                     [parameter] {
        static int angle = 0;
        const float anglef = qDegreesToRadians(float(angle));
        parameter->setValue(QColor::fromRgbF(fabs(qCos(anglef)), fabs(qSin(anglef)), 1.0f));
        angle += 10;
    });
    parameterAnimationTimer->start(16);

    // Torus
    Qt3DCore::QEntity *customMeshEntity = new Qt3DCore::QEntity(rootEntity);

    // Transform
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
    transform->setScale(8.0f);

    // Custom Mesh (TetraHedron)
    Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);

    Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(customGeometry);
    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(customGeometry);

    // 5 vertices of 3 vertices + 3 colors
    QByteArray vertexBufferData;
    vertexBufferData.resize(5 * (3 + 3) * sizeof(float));

    // Vertices
    QVector3D v0(-1.0f, 0.0f, -1.0f);
    QVector3D v1(1.0f, 0.0f, -1.0f);
    QVector3D v2(-1.0f, 0.0f, 1.0f);
    QVector3D v3(1.0f, 0.0f, 1.0f);
    QVector3D v4(0.0f, 2.0f, 0.0f);

    QVector3D red(1.0f, 0.0f, 0.0f);
    QVector3D green(0.0f, 1.0f, 0.0f);
    QVector3D blue(0.0f, 0.0f, 1.0f);
    QVector3D white(1.0f, 1.0f, 1.0f);
    QVector3D grey(0.5f, 0.5f, 0.5f);

    const QVector<QVector3D> vertices = QVector<QVector3D>()
            << v0 << red
            << v1 << green
            << v2 << blue
            << v3 << grey
            << v4 << white;

    memcpy(vertexBufferData.data(), vertices.constData(), vertices.size() * sizeof(QVector3D));
    vertexDataBuffer->setData(vertexBufferData);

    QByteArray indexBufferData;
    // 6 triangle faces
    indexBufferData.resize(6 * 3 * sizeof(ushort));
    ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

    rawIndexArray[0] = 0;
    rawIndexArray[1] = 1;
    rawIndexArray[2] = 2;

    rawIndexArray[3] = 2;
    rawIndexArray[4] = 1;
    rawIndexArray[5] = 3;

    rawIndexArray[6] = 2;
    rawIndexArray[7] = 3;
    rawIndexArray[8] = 4;

    rawIndexArray[9] = 3;
    rawIndexArray[10] = 1;
    rawIndexArray[11] = 4;

    rawIndexArray[12] = 1;
    rawIndexArray[13] = 0;
    rawIndexArray[14] = 4;

    rawIndexArray[15] = 0;
    rawIndexArray[16] = 2;
    rawIndexArray[17] = 4;

    indexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setCount(5);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    colorAttribute->setVertexSize(3);
    colorAttribute->setByteOffset(3 * sizeof(float));
    colorAttribute->setByteStride(6 * sizeof(float));
    colorAttribute->setCount(5);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setVertexSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(18);

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(colorAttribute);
    customGeometry->addAttribute(indexAttribute);

    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    customMeshRenderer->setGeometry(customGeometry);

    customMeshEntity->addComponent(customMeshRenderer);
    customMeshEntity->addComponent(transform);
    customMeshEntity->addComponent(material);

    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
