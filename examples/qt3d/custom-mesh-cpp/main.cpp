/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include <Qt3DRenderer/QStateSet>
#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QForwardRenderer>
#include <Qt3DRenderer/QPerVertexColorMaterial>

#include <Qt3DRenderer/QGeometryRenderer>
#include <Qt3DRenderer/QGeometry>
#include <Qt3DRenderer/QAttribute>
#include <Qt3DRenderer/QBuffer>

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
    Qt3D::QMaterial *material = new Qt3D::QPerVertexColorMaterial(rootEntity);

    // Torus
    Qt3D::QEntity *customMeshEntity = new Qt3D::QEntity(rootEntity);

    // Transform
    Qt3D::QTransform *transform = new Qt3D::QTransform;
    Qt3D::QScaleTransform *scaleTransform = new Qt3D::QScaleTransform;
    scaleTransform->setScale(8.0f);
    transform->addTransform(scaleTransform);

    // Custom Mesh (TetraHedron)
    Qt3D::QGeometryRenderer *customMeshRenderer = new Qt3D::QGeometryRenderer;
    Qt3D::QGeometry *customGeometry = new Qt3D::QGeometry(customMeshRenderer);

    Qt3D::QBuffer *vertexDataBuffer = new Qt3D::QBuffer(Qt3D::QBuffer::VertexBuffer, customGeometry);
    Qt3D::QBuffer *indexDataBuffer = new Qt3D::QBuffer(Qt3D::QBuffer::IndexBuffer, customGeometry);

    // vec3 for position
    // vec3 for colors
    // vec3 for normals

    /*          2
               /|\
              / | \
             / /3\ \
             0/___\ 1
    */

    // 4 distinct vertices
    QByteArray vertexBufferData;
    vertexBufferData.resize(4 * (3 + 3 + 3) * sizeof(float));

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

    QVector<QVector3D> vertices = QVector<QVector3D>()
            << v0 << n0 << red
            << v1 << n1 << blue
            << v2 << n2 << green
            << v3 << n3 << white;

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;

    Q_FOREACH (const QVector3D &v, vertices) {
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

    vertexDataBuffer->setData(vertexBufferData);
    indexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3D::QAttribute *positionAttribute = new Qt3D::QAttribute();
    positionAttribute->setAttributeType(Qt3D::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataType(Qt3D::QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(9 * sizeof(float));
    positionAttribute->setCount(4);
    positionAttribute->setName(Qt3D::QAttribute::defaultPositionAttributeName());

    Qt3D::QAttribute *normalAttribute = new Qt3D::QAttribute();
    normalAttribute->setAttributeType(Qt3D::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(vertexDataBuffer);
    normalAttribute->setDataType(Qt3D::QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(3 * sizeof(float));
    normalAttribute->setByteStride(9 * sizeof(float));
    normalAttribute->setCount(4);
    normalAttribute->setName(Qt3D::QAttribute::defaultNormalAttributeName());

    Qt3D::QAttribute *colorAttribute = new Qt3D::QAttribute();
    colorAttribute->setAttributeType(Qt3D::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setDataType(Qt3D::QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(6 * sizeof(float));
    colorAttribute->setByteStride(9 * sizeof(float));
    colorAttribute->setCount(4);
    colorAttribute->setName(Qt3D::QAttribute::defaultColorAttributeName());

    Qt3D::QAttribute *indexAttribute = new Qt3D::QAttribute();
    indexAttribute->setAttributeType(Qt3D::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setDataType(Qt3D::QAttribute::UnsignedShort);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(12);

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(normalAttribute);
    customGeometry->addAttribute(colorAttribute);
    customGeometry->addAttribute(indexAttribute);

    customMeshRenderer->setInstanceCount(1);
    customMeshRenderer->setBaseVertex(0);
    customMeshRenderer->setBaseInstance(0);
    customMeshRenderer->setPrimitiveType(Qt3D::QGeometryRenderer::Triangles);
    customMeshRenderer->setGeometry(customGeometry);
    // 4 faces of 3 points
    customMeshRenderer->setPrimitiveCount(12);

    customMeshEntity->addComponent(customMeshRenderer);
    customMeshEntity->addComponent(transform);
    customMeshEntity->addComponent(material);


    rootEntity->addComponent(frameGraph);

    engine.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
