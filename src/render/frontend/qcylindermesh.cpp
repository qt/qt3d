/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef _USE_MATH_DEFINES
# define _USE_MATH_DEFINES // For MSVC
#endif

#include "qcylindermesh.h"
#include "qbuffer.h"
#include "qattribute.h"
#include "qmeshdata.h"
#include <Qt3DRenderer/private/qabstractmesh_p.h>

#include <qmath.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class CylinderMeshFunctor : public QAbstractMeshFunctor
{
public:
    CylinderMeshFunctor(int rings, int slices, float radius, float length);
    QMeshDataPtr operator ()() Q_DECL_OVERRIDE;
    bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE;

private:
    int m_rings;
    int m_slices;
    float m_radius;
    float m_length;

};

class QCylinderMeshPrivate : public QAbstractMeshPrivate
{
    QCylinderMeshPrivate()
        : QAbstractMeshPrivate()
        , m_rings(16)
        , m_slices(16)
        , m_radius(1.0)
        , m_length(1.0)
    {}

    Q_DECLARE_PUBLIC(QCylinderMesh)
    int m_rings;
    int m_slices;
    float m_radius;
    float m_length;
};

void QCylinderMesh::copy(const QNode *ref)
{
    QAbstractMesh::copy(ref);
    const QCylinderMesh *mesh = static_cast<const QCylinderMesh*>(ref);
    d_func()->m_rings = mesh->d_func()->m_rings;
    d_func()->m_slices = mesh->d_func()->m_slices;
    d_func()->m_radius = mesh->d_func()->m_radius;
    d_func()->m_length = mesh->d_func()->m_length;
}

QCylinderMesh::QCylinderMesh(QNode *parent)
    : QAbstractMesh(*new QCylinderMeshPrivate, parent)
{
    update();
}


void QCylinderMesh::setRings(int rings)
{
    Q_D(QCylinderMesh);
    if (rings != d->m_rings) {
        d->m_rings = rings;
        emit ringsChanged(rings);
        update();
    }
}

void QCylinderMesh::setSlices(int slices)
{
    Q_D(QCylinderMesh);
    if (slices != d->m_slices) {
        d->m_slices = slices;
        emit slicesChanged(slices);
        update();
    }
}

void QCylinderMesh::setRadius(float radius)
{
    Q_D(QCylinderMesh);
    if (radius != d->m_radius) {
        d->m_radius = radius;
        emit radiusChanged(radius);
        update();
    }
}

void QCylinderMesh::setLength(float length)
{
    Q_D(QCylinderMesh);
    if (length != d->m_length) {
        d->m_length = length;
        emit lengthChanged(length);
        update();
    }
}

int QCylinderMesh::rings() const
{
    Q_D(const QCylinderMesh);
    return d->m_rings;
}

int QCylinderMesh::slices() const
{
    Q_D(const QCylinderMesh);
    return d->m_slices;
}

float QCylinderMesh::radius() const
{
    Q_D(const QCylinderMesh);
    return d->m_radius;
}

float QCylinderMesh::length() const
{
    Q_D(const QCylinderMesh);
    return d->m_length;
}

QMeshDataPtr assembleMesh(const QByteArray &verticesBytes, quint32 vertexSize, int verticesCount,
                                  const QByteArray &indicesBytes, int indicesCount)
{
    QMeshDataPtr mesh(new QMeshData(QMeshData::Triangles));

    BufferPtr verticesBuffer(new Buffer(QOpenGLBuffer::VertexBuffer));
    verticesBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    verticesBuffer->setData(verticesBytes);

    mesh->addAttribute(QMeshData::defaultPositionAttributeName(),
                       QAbstractAttributePtr(new Attribute(verticesBuffer, GL_FLOAT_VEC3,
                                                           verticesCount, 0, vertexSize)));
    quint32 offset = sizeof(float) * 3;

    mesh->addAttribute(QMeshData::defaultTextureCoordinateAttributeName(),
                       QAbstractAttributePtr(new Attribute(verticesBuffer, GL_FLOAT_VEC2,
                                                           verticesCount, offset, vertexSize)));
    offset += sizeof(float) * 2;

    mesh->addAttribute(QMeshData::defaultNormalAttributeName(),
                       QAbstractAttributePtr(new Attribute(verticesBuffer, GL_FLOAT_VEC3,
                                                           verticesCount, offset, vertexSize)));
    offset += sizeof(float) * 3;

    BufferPtr indicesBuffer(new Buffer(QOpenGLBuffer::IndexBuffer));
    indicesBuffer->setUsage(QOpenGLBuffer::StaticDraw);
    indicesBuffer->setData(indicesBytes);
    mesh->setIndexAttribute(AttributePtr(new Attribute(indicesBuffer, GL_UNSIGNED_SHORT, indicesCount, 0, 0)));

    mesh->computeBoundsFromAttribute(QMeshData::defaultPositionAttributeName());

    return mesh;
}

void createSides(float *&verticesPtr, quint16 *&indicesPtr,
                 int rings, int slices,
                 double radius, double length)
{
    const float dY = length / static_cast<float>(rings - 1);
    const float dTheta = (M_PI * 2) / static_cast<float>(slices);

    for (int ring = 0; ring < rings; ring++) {
        const float y = -length / 2.0f + static_cast<float>(ring) * dY;
        const int ringIndexStart = ring * (slices + 1);
        const int nextRingIndexStart = (ring + 1) * (slices + 1);

        for (int slice = 0; slice <= slices; slice++) {
            const float theta = static_cast<float>(slice) * dTheta;
            const float ct = qCos(theta);
            const float st = qSin(theta);

            *verticesPtr++ = radius * ct;
            *verticesPtr++ = y;
            *verticesPtr++ = radius * st;

            *verticesPtr++ = (y + length / 2.0) / length;
            *verticesPtr++ = theta / (M_PI * 2);

            QVector3D n(ct, 0.0f, st);
            n.normalize();
            *verticesPtr++ = n.x();
            *verticesPtr++ = n.y();
            *verticesPtr++ = n.z();

            if (slice == slices)
                continue;

            const int nextSlice = slice + 1;

            *indicesPtr++ = (ringIndexStart + slice);
            *indicesPtr++ = (nextRingIndexStart + slice);
            *indicesPtr++ = (ringIndexStart + nextSlice);
            *indicesPtr++ = (ringIndexStart + nextSlice);
            *indicesPtr++ = (nextRingIndexStart + slice);
            *indicesPtr++ = (nextRingIndexStart + nextSlice);
        }
    }
}

void createDisc(float *&verticesPtr, quint16 *&indicesPtr,
                int discCenterIndex, int slices,
                double radius, double yPosition)
{
    const float dTheta = (M_PI * 2) / static_cast<float>(slices);
    const double yNormal = (yPosition < 0.0f) ? -1.0f : 1.0f;

    *verticesPtr++ = 0.0f;
    *verticesPtr++ = yPosition;
    *verticesPtr++ = 0.0f;

    *verticesPtr++ = 1.0f;
    *verticesPtr++ = 0.0f;

    *verticesPtr++ = 0.0f;
    *verticesPtr++ = yNormal;
    *verticesPtr++ = 0.0f;


    for (int slice = 0; slice <= slices; slice++) {
        const float theta = static_cast<float>(slice) * dTheta;
        const float ct = qCos(theta);
        const float st = qSin(theta);

        *verticesPtr++ = radius * ct;
        *verticesPtr++ = yPosition;
        *verticesPtr++ = radius * st;

        *verticesPtr++ = 1.0f;
        *verticesPtr++ = theta / (M_PI * 2);

        *verticesPtr++ = 0.0f;
        *verticesPtr++ = yNormal;
        *verticesPtr++ = 0.0f;

        if (slice == slices)
            continue;

        const int nextSlice = slice + 1;

        *indicesPtr++ = discCenterIndex;
        *indicesPtr++ = (discCenterIndex + 1 + nextSlice);
        *indicesPtr++ = (discCenterIndex + 1 + slice);

        if (yNormal < 0.0f)
            qSwap(*(indicesPtr -1), *(indicesPtr - 2));
    }
}

QMeshDataPtr createCylinderMesh(double radius, double length,
                                        int rings, int slices)
{
    const int verticesCount  = (slices + 1) * rings + 2 * (slices + 1) + 2;
    // vec3 pos, vec2 texCoord, vec3 normal
    const quint32 vertexSize = (3 + 2 + 3) * sizeof(float);

    QByteArray verticesBytes;
    verticesBytes.resize(verticesCount * vertexSize);
    float *verticesPtr = reinterpret_cast<float*>(verticesBytes.data());

    const int facesCount = (slices * 2) * rings // two tris per side, for all rings
                         + slices * 2; // two caps
    const int indicesCount = facesCount * 3;
    const int indexSize = sizeof(quint16);
    Q_ASSERT(indicesCount < 65536);

    QByteArray indicesBytes;
    indicesBytes.resize(indicesCount * indexSize);
    quint16 *indicesPtr = reinterpret_cast<quint16*>(indicesBytes.data());

    createSides(verticesPtr, indicesPtr, rings, slices, radius, length);
    createDisc(verticesPtr, indicesPtr, rings * (slices + 1),
               slices, radius, -length / 2.0f);
    createDisc(verticesPtr, indicesPtr, rings * (slices + 1) + slices + 2,
               slices, radius, length / 2.0f);
    return assembleMesh(verticesBytes, vertexSize, verticesCount,
                        indicesBytes, indicesCount);
}

QAbstractMeshFunctorPtr QCylinderMesh::meshFunctor() const
{
    Q_D(const QCylinderMesh);
    return QAbstractMeshFunctorPtr(new CylinderMeshFunctor(d->m_rings, d->m_slices, d->m_radius, d->m_length));
}

CylinderMeshFunctor::CylinderMeshFunctor(int rings, int slices, float radius, float length)
    : QAbstractMeshFunctor()
    , m_rings(rings)
    , m_slices(slices)
    , m_radius(radius)
    , m_length(length)
{
}

QMeshDataPtr CylinderMeshFunctor::operator ()()
{
    return createCylinderMesh(m_radius, m_length, m_rings, m_slices);
}

bool CylinderMeshFunctor::operator ==(const QAbstractMeshFunctor &other) const
{
    const CylinderMeshFunctor *otherFunctor = dynamic_cast<const CylinderMeshFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
        return (otherFunctor->m_radius == m_radius &&
                otherFunctor->m_length == m_length &&
                otherFunctor->m_rings == m_rings &&
                otherFunctor->m_slices == m_slices);
    return false;
}

} // Qt3D

QT_END_NAMESPACE
