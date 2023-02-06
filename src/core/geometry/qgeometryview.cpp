// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgeometryview.h"
#include "qgeometryview_p.h"
#include "qgeometry_p.h"
#include "buffervisitor_p.h"

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/private/vector3d_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace {

class FindExtremePoints : public Buffer3fVisitor
{
public:
    FindExtremePoints()
        : Buffer3fVisitor()
          , xMin(0.0f), xMax(0.0f), yMin(0.0f), yMax(0.0f), zMin(0.0f), zMax(0.0f)
    { }

    float xMin, xMax, yMin, yMax, zMin, zMax;
    Vector3D xMinPt, xMaxPt, yMinPt, yMaxPt, zMinPt, zMaxPt;

    void visit(uint ndx, float x, float y, float z) override
    {
        if (ndx) {
            if (x < xMin) {
                xMin = x;
                xMinPt = Vector3D(x, y, z);
            }
            if (x > xMax) {
                xMax = x;
                xMaxPt = Vector3D(x, y, z);
            }
            if (y < yMin) {
                yMin = y;
                yMinPt = Vector3D(x, y, z);
            }
            if (y > yMax) {
                yMax = y;
                yMaxPt = Vector3D(x, y, z);
            }
            if (z < zMin) {
                zMin = z;
                zMinPt = Vector3D(x, y, z);
            }
            if (z > zMax) {
                zMax = z;
                zMaxPt = Vector3D(x, y, z);
            }
        } else {
            xMin = xMax = x;
            yMin = yMax = y;
            zMin = zMax = z;
            xMinPt = xMaxPt = yMinPt = yMaxPt = zMinPt = zMaxPt = Vector3D(x, y, z);
        }
    }
};

class FindMaxDistantPoint : public Buffer3fVisitor
{
public:
    FindMaxDistantPoint()
        : Buffer3fVisitor()
    { }

    float maxLengthSquared = 0.0f;
    bool setReferencePoint = false;
    bool hasNoPoints = true;
    Vector3D maxDistPt;
    Vector3D referencePt;

    void visit(uint ndx, float x, float y, float z) override
    {
        Q_UNUSED(ndx);
        const Vector3D p = Vector3D(x, y, z);

        if (hasNoPoints && setReferencePoint) {
            maxLengthSquared = 0.0f;
            referencePt = p;
        }
        const float lengthSquared = (p - referencePt).lengthSquared();
        if ( lengthSquared >= maxLengthSquared ) {
            maxDistPt = p;
            maxLengthSquared = lengthSquared;
        }
        hasNoPoints = false;
    }
};

}

bool BoundingVolumeCalculator::apply(QAttribute *positionAttribute,
                                     QAttribute *indexAttribute,
                                     int drawVertexCount,
                                     bool primitiveRestartEnabled,
                                     int primitiveRestartIndex)
{
    m_radius = -1.f;

    FindExtremePoints findExtremePoints;
    if (!findExtremePoints.apply(positionAttribute, indexAttribute, drawVertexCount,
                                 primitiveRestartEnabled, primitiveRestartIndex)) {
        return false;
    }

    m_min = QVector3D(findExtremePoints.xMin, findExtremePoints.yMin, findExtremePoints.zMin);
    m_max = QVector3D(findExtremePoints.xMax, findExtremePoints.yMax, findExtremePoints.zMax);

    FindMaxDistantPoint maxDistantPointY;
    maxDistantPointY.setReferencePoint = true;
    if (!maxDistantPointY.apply(positionAttribute, indexAttribute, drawVertexCount,
                                primitiveRestartEnabled, primitiveRestartIndex)) {
        return false;
    }
    if (maxDistantPointY.hasNoPoints)
        return false;

    //const Vector3D x = maxDistantPointY.referencePt;
    const Vector3D y = maxDistantPointY.maxDistPt;

    FindMaxDistantPoint maxDistantPointZ;
    maxDistantPointZ.setReferencePoint = false;
    maxDistantPointZ.referencePt = y;
    if (!maxDistantPointZ.apply(positionAttribute, indexAttribute, drawVertexCount,
                                primitiveRestartEnabled, primitiveRestartIndex))
        return false;
    const Vector3D z = maxDistantPointZ.maxDistPt;
    const Vector3D center = (y + z) * .5f;

    FindMaxDistantPoint maxDistantPointCenter;
    maxDistantPointCenter.setReferencePoint = false;
    maxDistantPointCenter.referencePt = center;
    if (!maxDistantPointCenter.apply(positionAttribute, indexAttribute, drawVertexCount,
                                     primitiveRestartEnabled, primitiveRestartIndex))
        return false;

    const float radius = (center - maxDistantPointCenter.maxDistPt).length();

    if (center == Vector3D{} && radius < 0.f)
        return false;

    m_radius = radius;
    m_center = QVector3D{ center.x(), center.y(), center.z() };

    return true;
}


QGeometryViewPrivate::QGeometryViewPrivate()
    : QNodePrivate()
    , m_instanceCount(1)
    , m_vertexCount(0)
    , m_indexOffset(0)
    , m_firstInstance(0)
    , m_firstVertex(0)
    , m_indexBufferByteOffset(0)
    , m_restartIndexValue(-1)
    , m_verticesPerPatch(0)
    , m_primitiveRestart(false)
    , m_geometry(nullptr)
    , m_primitiveType(QGeometryView::Triangles)
    , m_dirty(false)
{
}

QGeometryViewPrivate::~QGeometryViewPrivate()
{
}

QGeometryViewPrivate *QGeometryViewPrivate::get(QGeometryView *q)
{
    return q->d_func();
}

void QGeometryViewPrivate::update()
{
    if (!m_blockNotifications)
        m_dirty = true;
    QNodePrivate::update();
}

/*!
    \qmltype GeometryView
    \instantiates Qt3DCore::QGeometryView
    \inqmlmodule Qt3D.Core
    \inherits Node
    \since 6.0
    \brief Encapsulates geometry details.

    A GeometryView holds all the information necessary to handle
    a Geometry. A Geometry holds the coordinates of the geometry data -
    GeometryView specifies how to interpret that data.
 */

/*!
    \class Qt3DCore::QGeometryView
    \inmodule Qt3DCore
    \since 6.0
    \brief Encapsulates geometry details.

    A GeometryView holds all the information necessary to handle
    a Geometry. A Geometry holds the coordinates of the geometry data -
    GeometryView specifies how to interpret that data.
 */


/*!
    \enum QGeometryView::PrimitiveType

    The type of the primitive.

    \value Points   List of points
    \value Lines    List of lines
    \value LineLoop Connected group of lines connected at ends forming a loop
    \value LineStrip Connected group of lines
    \value Triangles List of triangles
    \value TriangleStrip List of connected triangles
    \value TriangleFan List of connected triagles where all triangles share the first vertex
    \value LinesAdjacency Allows geometry shader to access adjacent lines in a line list
    \value TrianglesAdjacency Allows geometry shader to access adjacent triangles in a triangle list
    \value LineStripAdjacency Allows geometry shader to access adjacent lines in a line strip
    \value TriangleStripAdjacency Allows geometry shader to access adjacent triangles in a triangle strip
    \value Patches Only primitive type accepted by tesselation shader where a patch consists of arbitrary number of vertices
 */

/*!
    \qmlproperty int GeometryView::instanceCount

    Holds the instance count.
 */

/*!
    \qmlproperty int GeometryView::vertexCount

    Holds the vertex count.
 */

/*!
    \qmlproperty int GeometryView::indexOffset

    Holds the base vertex.
 */

/*!
    \qmlproperty int GeometryView::firstInstance

    Holds the base instance.
 */

/*!
    \qmlproperty int GeometryView::firstVertex

    Holds the first vertex.
 */

/*!
    \qmlproperty int GeometryView::indexBufferByteOffset

    Holds the byte offset into the index buffer.
 */

/*!
    \qmlproperty int GeometryView::restartIndex

    Holds the restart index.
 */

/*!
    \qmlproperty int GeometryView::verticesPerPatch

    Holds vertices per patch.
 */

/*!
    \qmlproperty bool GeometryView::primitiveRestart

    Holds the primitive restart flag.
 */

/*!
    \qmlproperty Geometry GeometryView::geometry

    Holds the geometry.
 */

/*!
    \qmlproperty enumeration GeometryView::primitiveType

    Holds the primitive type.

    \list
    \li QGeometryView.Points
    \li QGeometryView.Lines
    \li QGeometryView.LineLoop
    \li QGeometryView.LineStrip
    \li QGeometryView.Triangles
    \li QGeometryView.TriangleStrip
    \li QGeometryView.TriangleFan
    \li QGeometryView.LinesAdjacency
    \li QGeometryView.TrianglesAdjacency
    \li QGeometryView.LineStripAdjacency
    \li QGeometryView.TriangleStripAdjacency
    \li QGeometryView.Patches
    \endlist
    \sa Qt3DCore::QGeometryView::PrimitiveType
 */


/*!
    Constructs a new QGeometryView with \a parent.
 */
QGeometryView::QGeometryView(QNode *parent)
    : QNode(*new QGeometryViewPrivate(), parent)
{
}

/*!
    \internal
 */
QGeometryView::~QGeometryView()
{
}

/*!
    \internal
 */
QGeometryView::QGeometryView(QGeometryViewPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

/*!
    \property QGeometryView::instanceCount

    Holds the instance count.
 */
int QGeometryView::instanceCount() const
{
    Q_D(const QGeometryView);
    return d->m_instanceCount;
}

/*!
    \property QGeometryView::vertexCount

    Holds the primitive count.
 */
int QGeometryView::vertexCount() const
{
    Q_D(const QGeometryView);
    return d->m_vertexCount;
}

/*!
    \property QGeometryView::indexOffset

    Holds the base vertex.
 */
int QGeometryView::indexOffset() const
{
    Q_D(const QGeometryView);
    return d->m_indexOffset;
}

/*!
    \property QGeometryView::firstInstance

    Holds the base instance.
 */
int QGeometryView::firstInstance() const
{
    Q_D(const QGeometryView);
    return d->m_firstInstance;
}

/*!
    \property QGeometryView::firstVertex

    Holds the base vertex.
 */
int QGeometryView::firstVertex() const
{
    Q_D(const QGeometryView);
    return d->m_firstVertex;
}

/*!
    \property QGeometryView::indexBufferByteOffset

    Holds the byte offset into the index buffer.
 */
int QGeometryView::indexBufferByteOffset() const
{
    Q_D(const QGeometryView);
    return d->m_indexBufferByteOffset;
}

/*!
    \property QGeometryView::restartIndexValue

    Holds the restart index.
 */
int QGeometryView::restartIndexValue() const
{
    Q_D(const QGeometryView);
    return d->m_restartIndexValue;
}

/*!
    \property QGeometryView::verticesPerPatch

    Holds vertices per patch.
 */
int QGeometryView::verticesPerPatch() const
{
    Q_D(const QGeometryView);
    return d->m_verticesPerPatch;
}

/*!
    \property QGeometryView::primitiveRestartEnabled

    Holds the primitive restart flag.
 */
bool QGeometryView::primitiveRestartEnabled() const
{
    Q_D(const QGeometryView);
    return d->m_primitiveRestart;
}

/*!
    \property QGeometryView::geometry

    Holds the geometry.
 */
QGeometry *QGeometryView::geometry() const
{
    Q_D(const QGeometryView);
    return d->m_geometry;
}

/*!
    \property QGeometryView::primitiveType

    Holds the primitive type.
 */
QGeometryView::PrimitiveType QGeometryView::primitiveType() const
{
    Q_D(const QGeometryView);
    return d->m_primitiveType;
}

void QGeometryView::setInstanceCount(int instanceCount)
{
    Q_D(QGeometryView);
    if (d->m_instanceCount == instanceCount)
        return;

    d->m_instanceCount = instanceCount;
    emit instanceCountChanged(instanceCount);
}

void QGeometryView::setVertexCount(int vertexCount)
{
    Q_D(QGeometryView);
    if (d->m_vertexCount == vertexCount)
        return;

    d->m_vertexCount = vertexCount;
    emit vertexCountChanged(vertexCount);
}

void QGeometryView::setIndexOffset(int indexOffset)
{
    Q_D(QGeometryView);
    if (d->m_indexOffset == indexOffset)
        return;

    d->m_indexOffset = indexOffset;
    emit indexOffsetChanged(indexOffset);
}

void QGeometryView::setFirstInstance(int firstInstance)
{
    Q_D(QGeometryView);
    if (d->m_firstInstance == firstInstance)
        return;

    d->m_firstInstance = firstInstance;
    emit firstInstanceChanged(firstInstance);
}

void QGeometryView::setFirstVertex(int firstVertex)
{
    Q_D(QGeometryView);
    if (d->m_firstVertex == firstVertex)
        return;

    d->m_firstVertex = firstVertex;
    emit firstVertexChanged(firstVertex);
}

void QGeometryView::setIndexBufferByteOffset(int offset)
{
    Q_D(QGeometryView);
    if (d->m_indexBufferByteOffset == offset)
        return;

    d->m_indexBufferByteOffset = offset;
    emit indexBufferByteOffsetChanged(offset);
}

void QGeometryView::setRestartIndexValue(int index)
{
    Q_D(QGeometryView);
    if (index == d->m_restartIndexValue)
        return;

    d->m_restartIndexValue = index;
    emit restartIndexValueChanged(index);
}

void QGeometryView::setVerticesPerPatch(int verticesPerPatch)
{
    Q_D(QGeometryView);
    if (d->m_verticesPerPatch != verticesPerPatch) {
        d->m_verticesPerPatch = verticesPerPatch;
        emit verticesPerPatchChanged(verticesPerPatch);
    }
}

void QGeometryView::setPrimitiveRestartEnabled(bool enabled)
{
    Q_D(QGeometryView);
    if (enabled == d->m_primitiveRestart)
        return;

    d->m_primitiveRestart = enabled;
    emit primitiveRestartEnabledChanged(enabled);
}

void QGeometryView::setGeometry(QGeometry *geometry)
{
    Q_D(QGeometryView);
    if (d->m_geometry == geometry)
        return;

    if (d->m_geometry)
        d->unregisterDestructionHelper(d->m_geometry);

    if (geometry && !geometry->parent())
        geometry->setParent(this);

    d->m_geometry = geometry;

    // Ensures proper bookkeeping
    if (d->m_geometry)
        d->registerDestructionHelper(d->m_geometry, &QGeometryView::setGeometry, d->m_geometry);

    emit geometryChanged(geometry);
}

void QGeometryView::setPrimitiveType(QGeometryView::PrimitiveType primitiveType)
{
    Q_D(QGeometryView);
    if (d->m_primitiveType == primitiveType)
        return;

    d->m_primitiveType = primitiveType;
    emit primitiveTypeChanged(primitiveType);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qgeometryview.cpp"
