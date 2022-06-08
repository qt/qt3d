// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPerVertexColorMaterial>

#include <Qt3DRender/QGeometryRenderer>

#include <QPropertyAnimation>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qorbitcameracontroller.h>

namespace {

struct IndirectElementDrawBuffer{ // Element Indirect
    uint count;
    uint instancesCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

struct IndirectArrayDrawBuffer{ // Array Indirect
    uint count;
    uint instancesCount;
    uint first;
    uint baseInstance;
};

} // anonymous

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;
    view.defaultFrameGraph()->setClearColor(QColor::fromRgbF(0.0, 0.5, 1.0, 1.0));

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *cameraEntity = view.camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 40.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(cameraEntity);

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);

    // vec3 for position
    // vec3 for colors
    // vec3 for normals

    /*          2
               /|\
              / | \
             / /3\ \
             0/___\ 1
    */
    // Vertices
    QVector3D v0(-1.0f, 0.0f, -1.0f);
    QVector3D v1(1.0f, 0.0f, -1.0f);
    QVector3D v2(0.0f, 1.0f, 0.0f);
    QVector3D v3(0.0f, 0.0f, 1.0f);

    // Faces Normals
    QVector3D n023 = QVector3D::normal(v0, v2, v3);
    QVector3D n012 = QVector3D::normal(v0, v1, v2);
    QVector3D n310 = QVector3D::normal(v3, v1, v0);
    QVector3D n132 = QVector3D::normal(v1, v3, v2);

    // Vector Normals
    QVector3D n0 = QVector3D(n023 + n012 + n310).normalized();
    QVector3D n1 = QVector3D(n132 + n012 + n310).normalized();
    QVector3D n2 = QVector3D(n132 + n012 + n023).normalized();
    QVector3D n3 = QVector3D(n132 + n310 + n023).normalized();

    // Colors
    QVector3D red(1.0f, 0.0f, 0.0f);
    QVector3D green(0.0f, 1.0f, 0.0f);
    QVector3D blue(0.0f, 0.0f, 1.0f);
    QVector3D white(1.0f, 1.0f, 1.0f);

    const QList<QVector3D> vertices = { v0, n0, red,
                                        v1, n1, blue,
                                        v2, n2, green,
                                        v3, n3, white };

    //     DrawElementIndirect
    {
        Qt3DCore::QEntity *customDrawElementIndirectEntity = new Qt3DCore::QEntity(rootEntity);

        // Transform
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setScale(8.0f);
        transform->setTranslation(QVector3D(-10.0f, 0.0, 0.0f));

        // Custom Mesh (TetraHedron)
        Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
        Qt3DCore::QGeometry *customGeometry = new Qt3DCore::QGeometry(customMeshRenderer);

        Qt3DCore::QBuffer *vertexDataBuffer = new Qt3DCore::QBuffer(customGeometry);
        Qt3DCore::QBuffer *indexDataBuffer = new Qt3DCore::QBuffer(customGeometry);
        Qt3DCore::QBuffer *indirectDrawDataBuffer = new Qt3DCore::QBuffer(customGeometry);

        // 4 distinct vertices
        QByteArray vertexBufferData;
        vertexBufferData.resize(4 * (3 + 3 + 3) * sizeof(float));

        float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
        int idx = 0;

        for (const QVector3D &v : vertices) {
            rawVertexArray[idx++] = v.x();
            rawVertexArray[idx++] = v.y();
            rawVertexArray[idx++] = v.z();
        }

        // Indices (12)
        QByteArray indexBufferData;
        indexBufferData.resize(4 * 3 * sizeof(ushort));
        ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

        // Front
        rawIndexArray[0] = 0;
        rawIndexArray[1] = 1;
        rawIndexArray[2] = 2;
        // Bottom
        rawIndexArray[3] = 3;
        rawIndexArray[4] = 1;
        rawIndexArray[5] = 0;
        // Left
        rawIndexArray[6] = 0;
        rawIndexArray[7] = 2;
        rawIndexArray[8] = 3;
        // Right
        rawIndexArray[9] = 1;
        rawIndexArray[10] = 3;
        rawIndexArray[11] = 2;


        QByteArray indirectBufferData;
        indirectBufferData.resize(sizeof(IndirectElementDrawBuffer));
        IndirectElementDrawBuffer *indirectStruct = reinterpret_cast<IndirectElementDrawBuffer *>(indirectBufferData.data());
        indirectStruct->count = 12U;
        indirectStruct->firstIndex = 0U;
        indirectStruct->baseInstance = 0U;
        indirectStruct->baseVertex = 0U;
        indirectStruct->instancesCount = 1U;

        vertexDataBuffer->setData(vertexBufferData);
        indexDataBuffer->setData(indexBufferData);
        indirectDrawDataBuffer->setData(indirectBufferData);

        // Attributes
        Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute();
        positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        positionAttribute->setBuffer(vertexDataBuffer);
        positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setByteOffset(0);
        positionAttribute->setByteStride(9 * sizeof(float));
        positionAttribute->setCount(4);
        positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());

        Qt3DCore::QAttribute *normalAttribute = new Qt3DCore::QAttribute();
        normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        normalAttribute->setBuffer(vertexDataBuffer);
        normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        normalAttribute->setVertexSize(3);
        normalAttribute->setByteOffset(3 * sizeof(float));
        normalAttribute->setByteStride(9 * sizeof(float));
        normalAttribute->setCount(4);
        normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());

        Qt3DCore::QAttribute *colorAttribute = new Qt3DCore::QAttribute();
        colorAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        colorAttribute->setBuffer(vertexDataBuffer);
        colorAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        colorAttribute->setVertexSize(3);
        colorAttribute->setByteOffset(6 * sizeof(float));
        colorAttribute->setByteStride(9 * sizeof(float));
        colorAttribute->setCount(4);
        colorAttribute->setName(Qt3DCore::QAttribute::defaultColorAttributeName());

        Qt3DCore::QAttribute *indexAttribute = new Qt3DCore::QAttribute();
        indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
        indexAttribute->setBuffer(indexDataBuffer);
        indexAttribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedShort);
        indexAttribute->setVertexSize(1);
        indexAttribute->setByteOffset(0);
        indexAttribute->setByteStride(0);
        indexAttribute->setCount(12);

        Qt3DCore::QAttribute *indirectAttribute = new Qt3DCore::QAttribute();
        indirectAttribute->setAttributeType(Qt3DCore::QAttribute::DrawIndirectAttribute);
        indirectAttribute->setBuffer(indirectDrawDataBuffer);
        indirectAttribute->setByteOffset(0);
        indirectAttribute->setByteStride(0);
        indirectAttribute->setCount(1);

        customGeometry->addAttribute(positionAttribute);
        customGeometry->addAttribute(normalAttribute);
        customGeometry->addAttribute(colorAttribute);
        customGeometry->addAttribute(indexAttribute);
        customGeometry->addAttribute(indirectAttribute);

        customMeshRenderer->setInstanceCount(1);
        customMeshRenderer->setIndexOffset(0);
        customMeshRenderer->setFirstInstance(0);
        customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
        customMeshRenderer->setGeometry(customGeometry);
        // 4 faces of 3 points
        customMeshRenderer->setVertexCount(12);

        customDrawElementIndirectEntity->addComponent(customMeshRenderer);
        customDrawElementIndirectEntity->addComponent(transform);
        customDrawElementIndirectEntity->addComponent(material);
    }

    // DrawArrayIndirect
    {
        Qt3DCore::QEntity *customDrawArrayIndirectEntity = new Qt3DCore::QEntity(rootEntity);

        // Transform
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setScale(8.0f);
        transform->setTranslation(QVector3D(10.0f, 0.0, 0.0f));

        // Custom Mesh (TetraHedron)
        Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
        Qt3DCore::QGeometry *customGeometry = new Qt3DCore::QGeometry(customMeshRenderer);

        Qt3DCore::QBuffer *vertexDataBuffer = new Qt3DCore::QBuffer(customGeometry);
        Qt3DCore::QBuffer *indirectDrawDataBuffer = new Qt3DCore::QBuffer(customGeometry);


        QVector<QVector3D> orderedPositionVertices;
        // 12 vertices with position, normal, color
        orderedPositionVertices.reserve(12 * (1 + 1 + 1));

        orderedPositionVertices << v0 << n0 << red;
        orderedPositionVertices << v1 << n1 << red;
        orderedPositionVertices << v2 << n2 << red;

        orderedPositionVertices << v3 << n3 << green;
        orderedPositionVertices << v1 << n1 << green;
        orderedPositionVertices << v0 << n0 << green;

        orderedPositionVertices << v0 << n0 << blue;
        orderedPositionVertices << v2 << n2 << blue;
        orderedPositionVertices << v3 << n3 << blue;

        orderedPositionVertices << v1 << n1 << white;
        orderedPositionVertices << v3 << n3 << white;
        orderedPositionVertices << v2 << n2 << white;

        QByteArray vertexBufferData;
        vertexBufferData.resize(orderedPositionVertices.size() * sizeof(QVector3D));
        memcpy(vertexBufferData.data(), orderedPositionVertices.data(), orderedPositionVertices.size() * sizeof(QVector3D));

        QByteArray indirectBufferData;
        indirectBufferData.resize(sizeof(IndirectArrayDrawBuffer));
        IndirectArrayDrawBuffer *indirectStruct = reinterpret_cast<IndirectArrayDrawBuffer *>(indirectBufferData.data());
        indirectStruct->count = 12U;
        indirectStruct->baseInstance = 0U;
        indirectStruct->instancesCount = 1U;
        indirectStruct->first = 0U;

        vertexDataBuffer->setData(vertexBufferData);
        indirectDrawDataBuffer->setData(indirectBufferData);

        // Attributes
        Qt3DCore::QAttribute *positionAttribute = new Qt3DCore::QAttribute();
        positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        positionAttribute->setBuffer(vertexDataBuffer);
        positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setByteOffset(0);
        positionAttribute->setByteStride(9 * sizeof(float));
        positionAttribute->setCount(12);
        positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());

        Qt3DCore::QAttribute *normalAttribute = new Qt3DCore::QAttribute();
        normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        normalAttribute->setBuffer(vertexDataBuffer);
        normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        normalAttribute->setVertexSize(3);
        normalAttribute->setByteOffset(3 * sizeof(float));
        normalAttribute->setByteStride(9 * sizeof(float));
        normalAttribute->setCount(12);
        normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());

        Qt3DCore::QAttribute *colorAttribute = new Qt3DCore::QAttribute();
        colorAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
        colorAttribute->setBuffer(vertexDataBuffer);
        colorAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        colorAttribute->setVertexSize(3);
        colorAttribute->setByteOffset(6 * sizeof(float));
        colorAttribute->setByteStride(9 * sizeof(float));
        colorAttribute->setCount(12);
        colorAttribute->setName(Qt3DCore::QAttribute::defaultColorAttributeName());

        Qt3DCore::QAttribute *indirectAttribute = new Qt3DCore::QAttribute();
        indirectAttribute->setAttributeType(Qt3DCore::QAttribute::DrawIndirectAttribute);
        indirectAttribute->setBuffer(indirectDrawDataBuffer);
        indirectAttribute->setByteOffset(0);
        indirectAttribute->setByteStride(0);
        indirectAttribute->setCount(1);

        customGeometry->addAttribute(positionAttribute);
        customGeometry->addAttribute(normalAttribute);
        customGeometry->addAttribute(colorAttribute);
        customGeometry->addAttribute(indirectAttribute);

        customMeshRenderer->setInstanceCount(1);
        customMeshRenderer->setIndexOffset(0);
        customMeshRenderer->setFirstInstance(0);
        customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
        customMeshRenderer->setGeometry(customGeometry);
        // 4 faces of 3 points
        customMeshRenderer->setVertexCount(12);

        customDrawArrayIndirectEntity->addComponent(customMeshRenderer);
        customDrawArrayIndirectEntity->addComponent(transform);
        customDrawArrayIndirectEntity->addComponent(material);
    }

    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
