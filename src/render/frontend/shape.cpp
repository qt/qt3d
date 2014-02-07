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

#define _USE_MATH_DEFINES // For MSVC
#include "shape.h"

#include <cmath>

#include <QDebug>

namespace Qt3D {

namespace {

MeshDataPtr createSphereMesh(double radius, int rings, int slices, bool hasTangents)
{
    MeshDataPtr mesh(new MeshData(GL_TRIANGLES));

    int nVerts  = ( slices + 1 ) * ( rings + 1 ); // One extra line of latitude
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    quint32 elementSize = 3 + 2 + 3 + (hasTangents ? 4 : 0);
    quint32 stride = elementSize * sizeof(float);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float dTheta = (M_PI * 2) / static_cast<float>( slices );
    const float dPhi = M_PI / static_cast<float>( rings );
    const float du = 1.0f / static_cast<float>( slices );
    const float dv = 1.0f / static_cast<float>( rings );

    // Iterate over latitudes (rings)
    for ( int lat = 0; lat < rings + 1; ++lat )
    {
        const float phi = M_PI_2 - static_cast<float>( lat ) * dPhi;
        const float cosPhi = cosf( phi );
        const float sinPhi = sinf( phi );
        const float v = 1.0f - static_cast<float>( lat ) * dv;

        // Iterate over longitudes (slices)
        for ( int lon = 0; lon < slices + 1; ++lon )
        {
            const float theta = static_cast<float>( lon ) * dTheta;
            const float cosTheta = cosf( theta );
            const float sinTheta = sinf( theta );
            const float u = static_cast<float>( lon ) * du;

            *fptr++ = radius * cosTheta * cosPhi;
            *fptr++ = radius * sinPhi;
            *fptr++ = radius * sinTheta * cosPhi;

            *fptr++ = u;
            *fptr++ = v;

            *fptr++   = cosTheta * cosPhi;
            *fptr++ = sinPhi;
            *fptr++ = sinTheta * cosPhi;

            if (hasTangents) {
                *fptr++ = sinTheta;
                *fptr++ = 0.0;
                *fptr++ = -cosTheta;
                *fptr++ = 1.0;
            }
        }
    }

    BufferPtr buf(new Buffer(QOpenGLBuffer::VertexBuffer));
    buf->setUsage(QOpenGLBuffer::StaticDraw);
    buf->setData(bufferBytes);

    mesh->addAttribute("position", new Attribute(buf, GL_FLOAT_VEC3, nVerts, 0, stride));
    quint32 offset = sizeof(float) * 3;

    mesh->addAttribute("texcoord", new Attribute(buf, GL_FLOAT_VEC2, nVerts, offset, stride));
    offset += sizeof(float) * 2;

    mesh->addAttribute("normal", new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride));
    offset += sizeof(float) * 3;

    if (hasTangents) {
        mesh->addAttribute("tangent", new Attribute(buf, GL_FLOAT_VEC4, nVerts, offset, stride));
        offset += sizeof(float) * 4;
    }

    int faces = (slices * 2) * (rings - 1); // two tris per slice, for all middle rings
    faces += 2 * slices; // tri per slice for both top and bottom

    QByteArray indexBytes;
    int indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    // top cap
    {
        const int nextRingStartIndex = slices + 1;
        for ( int j = 0; j < slices; ++j )
        {
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = 0;
            *indexPtr++ = nextRingStartIndex + j + 1;
        }
    }

    for ( int i = 1; i < (rings - 1); ++i )
    {
        const int ringStartIndex = i * ( slices + 1 );
        const int nextRingStartIndex = ( i + 1 ) * ( slices + 1 );

        for ( int j = 0; j < slices; ++j )
        {
            // Split the quad into two triangles
            *indexPtr++ = ringStartIndex + j;
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = nextRingStartIndex + j;
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex + j + 1;
        }
    }

    // bottom cap
    {
        const int ringStartIndex = (rings - 1) * ( slices + 1);
        const int nextRingStartIndex = (rings) * ( slices + 1);
        for ( int j = 0; j < slices; ++j )
        {
            *indexPtr++ = ringStartIndex + j + 1;
            *indexPtr++ = nextRingStartIndex;
            *indexPtr++ = ringStartIndex + j;
        }
    }

    BufferPtr indexBuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    indexBuffer->setData(indexBytes);
    mesh->setIndexAttr(AttributePtr(new Attribute(indexBuffer, GL_UNSIGNED_SHORT, indices, 0, 0)));

    mesh->computeBoundsFromAttribute("position");
    qDebug() << "computed sphere bounds is:" << mesh->boundingBox();

    return mesh;
}


MeshDataPtr createTorusMesh(double radius, double minorRadius,
                            int rings, int sides)
{
    MeshDataPtr mesh(new MeshData(GL_TRIANGLES));

    int nVerts  = ( sides + 1 ) * ( rings + 1 );
    QByteArray bufferBytes;
    // vec3 pos, vec2 texCoord, vec3 normal
    quint32 elementSize = 3 + 2 + 3;
    quint32 stride = elementSize * sizeof(float);
    bufferBytes.resize(stride * nVerts);

    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    float ringFactor = (M_PI * 2) / static_cast<float>( rings );
    float sideFactor = (M_PI * 2) / static_cast<float>( sides );

    for ( int ring = 0; ring <= rings; ring++ )
    {
        float u = ring * ringFactor;
        float cu = cos( u );
        float su = sin( u );

        for ( int side = 0; side < sides; side++ )
        {
            float v = side * sideFactor;
            float cv = cos( v );
            float sv = sin( v );
            float r = ( radius + minorRadius * cv );

            *fptr++ = r * cu;
            *fptr++ = r * su;
            *fptr++ = minorRadius * sv;


            *fptr++ = u / (M_PI * 2);
            *fptr++ = v / (M_PI * 2);

            QVector3D n(cv * cu * r, cv * su * r, sv * r);
            n.normalize();
            *fptr++ = n.x();
            *fptr++ = n.y();
            *fptr++ = n.z();
        }
    }

    BufferPtr buf(new Buffer(QOpenGLBuffer::VertexBuffer));
    buf->setUsage(QOpenGLBuffer::StaticDraw);
    buf->setData(bufferBytes);

    mesh->addAttribute("position", new Attribute(buf, GL_FLOAT_VEC3, nVerts, 0, stride));
    quint32 offset = sizeof(float) * 3;

    mesh->addAttribute("texcoord", new Attribute(buf, GL_FLOAT_VEC2, nVerts, offset, stride));
    offset += sizeof(float) * 2;

    mesh->addAttribute("normal", new Attribute(buf, GL_FLOAT_VEC3, nVerts, offset, stride));
    offset += sizeof(float) * 3;

    QByteArray indexBytes;
    int faces = (sides * 2) * rings; // two tris per side, for all rings
    int indices = faces * 3;
    Q_ASSERT(indices < 65536);
    indexBytes.resize(indices * sizeof(quint16));
    quint16* indexPtr = reinterpret_cast<quint16*>(indexBytes.data());

    for ( int ring = 0; ring < rings; ring++ )
    {
        int ringStart = ring * sides;
        int nextRingStart = ( ring + 1 ) * sides;
        for ( int side = 0; side < sides; side++ )
        {
            int nextSide = ( side + 1 ) % sides;
            *indexPtr++ = ( ringStart + side );
            *indexPtr++ = ( nextRingStart + side );
            *indexPtr++ = ( nextRingStart + nextSide );
            *indexPtr++ = ringStart + side;
            *indexPtr++ = nextRingStart + nextSide;
            *indexPtr++ = ( ringStart + nextSide );
        }
    }

    BufferPtr indexBuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    indexBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    indexBuffer->setData(indexBytes);
    mesh->setIndexAttr(AttributePtr(new Attribute(indexBuffer, GL_UNSIGNED_SHORT, indices, 0, 0)));

    mesh->computeBoundsFromAttribute("position");

    return mesh;
}

} // anonymous namespace

Shape::Shape(Node *parent) :
    Qt3D::Component(parent),
    m_generateTangents(false),
    m_type(ShapeSphere),
    m_rings(16),
    m_slices(16),
    m_radius(1.0),
    m_minorRadius(1.0)
{
}

Shape::ShapeType Shape::type() const
{
    return m_type;
}

MeshDataPtr Shape::data() const
{
    if (m_type == ShapeTorus)
        return createTorusMesh(m_radius, m_minorRadius, m_rings, m_slices);

    return createSphereMesh(m_radius, m_rings, m_slices, m_generateTangents);
}

int Shape::rings() const
{
    return m_rings;
}

int Shape::slices() const
{
    return m_slices;
}

bool Shape::generateTangents() const
{
    return m_generateTangents;
}

double Shape::radius() const
{
    return m_radius;
}

double Shape::minorRadius() const
{
    return m_minorRadius;
}

void Shape::setType(Shape::ShapeType arg)
{
    if (m_type != arg) {
        m_type = arg;
        m_rebuildMesh = true;
        emit typeChanged(arg);
    }
}

void Shape::setRings(int arg)
{
    if (m_rings == arg)
        return;

    m_rings = arg;
    m_rebuildMesh = true;
    emit shapeChanged();
}

void Shape::setSlices(int arg)
{
    if (m_slices == arg)
        return;

    m_slices = arg;
    m_rebuildMesh = true;
    emit shapeChanged();
}

void Shape::setGenerateTangents(bool gen)
{
    if (m_generateTangents == gen)
        return;

    m_generateTangents = gen;
    m_rebuildMesh = true;
    emit shapeChanged();
}

void Shape::setRadius(double arg)
{
    if (m_radius != arg) {
        m_radius = arg;
        m_rebuildMesh = true;
        emit shapeChanged();
    }
}

void Shape::setMinorRadius(double arg)
{
    if (m_minorRadius != arg) {
        m_minorRadius = arg;
        m_rebuildMesh = true;
        emit shapeChanged();
    }
}

} // namespace Qt3D
