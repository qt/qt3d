// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qgeometryrenderer.h>

#include <Qt3DExtras/private/qtext2dmaterial_p.h>

#include "distancefieldtextrenderer_p.h"
#include "distancefieldtextrenderer_p_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

using namespace Qt3DCore;

DistanceFieldTextRendererPrivate::DistanceFieldTextRendererPrivate()
    : m_renderer(nullptr)
    , m_geometry(nullptr)
    , m_positionAttr(nullptr)
    , m_texCoordAttr(nullptr)
    , m_indexAttr(nullptr)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
    , m_material(nullptr)
{
}

DistanceFieldTextRendererPrivate::~DistanceFieldTextRendererPrivate()
{
}

void DistanceFieldTextRendererPrivate::init()
{
    Q_Q(DistanceFieldTextRenderer);

    m_renderer = new Qt3DRender::QGeometryRenderer(q);
    m_renderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);

    m_geometry = new Qt3DCore::QGeometry(m_renderer);
    m_renderer->setGeometry(m_geometry);

    m_vertexBuffer = new Qt3DCore::QBuffer(m_geometry);
    m_indexBuffer = new Qt3DCore::QBuffer(m_geometry);

    m_positionAttr = new Qt3DCore::QAttribute(m_geometry);
    m_positionAttr->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    m_positionAttr->setVertexBaseType(Qt3DCore::QAttribute::Float);
    m_positionAttr->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    m_positionAttr->setVertexSize(3);
    m_positionAttr->setByteStride(5 * sizeof(float));
    m_positionAttr->setByteOffset(0);
    m_positionAttr->setBuffer(m_vertexBuffer);

    m_texCoordAttr = new Qt3DCore::QAttribute(m_geometry);
    m_texCoordAttr->setName(Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName());
    m_texCoordAttr->setVertexBaseType(Qt3DCore::QAttribute::Float);
    m_texCoordAttr->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    m_texCoordAttr->setVertexSize(2);
    m_texCoordAttr->setByteStride(5 * sizeof(float));
    m_texCoordAttr->setByteOffset(3 * sizeof(float));
    m_texCoordAttr->setBuffer(m_vertexBuffer);

    m_indexAttr = new Qt3DCore::QAttribute(m_geometry);
    m_indexAttr->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
    m_indexAttr->setVertexBaseType(Qt3DCore::QAttribute::UnsignedShort);
    m_indexAttr->setBuffer(m_indexBuffer);

    m_geometry->addAttribute(m_positionAttr);
    m_geometry->setBoundingVolumePositionAttribute(m_positionAttr);
    m_geometry->addAttribute(m_texCoordAttr);
    m_geometry->addAttribute(m_indexAttr);

    m_material = new QText2DMaterial(q);

    q->addComponent(m_renderer);
    q->addComponent(m_material);
}

DistanceFieldTextRenderer::DistanceFieldTextRenderer(QNode *parent)
    : QEntity(*new DistanceFieldTextRendererPrivate(), parent)
{
    Q_D(DistanceFieldTextRenderer);
    d->init();
}

DistanceFieldTextRenderer::~DistanceFieldTextRenderer()
{
}

void DistanceFieldTextRenderer::setGlyphData(Qt3DRender::QAbstractTexture *glyphTexture,
                                             const std::vector<float> &vertexData,
                                             const std::vector<quint16> &indexData)
{
    Q_D(DistanceFieldTextRenderer);

    const size_t vertexCount = vertexData.size() / 5;

    d->m_vertexBuffer->setData(QByteArray((char*) vertexData.data(), vertexData.size() * sizeof(float)));
    d->m_indexBuffer->setData(QByteArray((char*) indexData.data(), indexData.size() * sizeof(quint16)));
    d->m_positionAttr->setCount(uint(vertexCount));
    d->m_texCoordAttr->setCount(uint(vertexCount));
    d->m_indexAttr->setCount(uint(indexData.size()));

    d->m_material->setDistanceFieldTexture(glyphTexture);
}

void DistanceFieldTextRenderer::setColor(const QColor &color)
{
    Q_D(DistanceFieldTextRenderer);
    d->m_material->setColor(color);
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_distancefieldtextrenderer_p.cpp"
