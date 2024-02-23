// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "tessellatedquadmesh.h"

#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qgeometry.h>

class TessellatedGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
public:
    TessellatedGeometry(Qt3DCore::QNode *parent = nullptr)
        : Qt3DCore::QGeometry(parent)
        , m_positionAttribute(new Qt3DCore::QAttribute(this))
        , m_vertexBuffer(new Qt3DCore::QBuffer(this))
    {
        const float positionData[] = {
            -0.8f, -0.8f, 0.0f,
             0.8f, -0.8f, 0.0f,
             0.8f,  0.8f, 0.0f,
            -0.8f,  0.8f, 0.0f
        };

        const int nVerts = 4;
        const int size = nVerts * 3 * sizeof(float);
        QByteArray positionBytes;
        positionBytes.resize(size);
        memcpy(positionBytes.data(), positionData, size);

        m_vertexBuffer->setData(positionBytes);

        m_positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
        m_positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
        m_positionAttribute->setVertexSize(3);
        m_positionAttribute->setCount(nVerts);
        m_positionAttribute->setByteStride(3 * sizeof(float));
        m_positionAttribute->setBuffer(m_vertexBuffer);

        addAttribute(m_positionAttribute);
    }

private:
    Qt3DCore::QAttribute *m_positionAttribute;
    Qt3DCore::QBuffer *m_vertexBuffer;
};

TessellatedQuadMesh::TessellatedQuadMesh(Qt3DCore::QNode *parent)
    : Qt3DRender::QGeometryRenderer(parent)
{
    setPrimitiveType(Qt3DRender::QGeometryRenderer::Patches);
    setVerticesPerPatch(4);
    setGeometry(new TessellatedGeometry(this));
}

#include "tessellatedquadmesh.moc"
