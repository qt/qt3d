// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgeometryrenderer.h"
#include "qgeometryrenderer_p.h"

#include <private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QGeometryRendererPrivate::QGeometryRendererPrivate()
    : Qt3DCore::QBoundingVolumePrivate()
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
    , m_primitiveType(QGeometryRenderer::Triangles)
    , m_sortIndex(-1.f)
{
    m_primaryProvider = false;
}

QGeometryRendererPrivate::~QGeometryRendererPrivate()
{
}

void QGeometryRendererPrivate::setView(QGeometryView *view)
{
    Q_Q(QGeometryRenderer);
    if (m_view == view)
        return;

    if (m_view)
        m_view->disconnect(q);

    QBoundingVolumePrivate::setView(view);

    // Ensures proper bookkeeping
    if (m_view) {
        QObject::connect(view, &QGeometryView::instanceCountChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::vertexCountChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::indexOffsetChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::firstInstanceChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::firstVertexChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::indexBufferByteOffsetChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::restartIndexValueChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::verticesPerPatchChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::primitiveRestartEnabledChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::geometryChanged, q, [this]() { update(); });
        QObject::connect(view, &QGeometryView::primitiveTypeChanged, q, [this]() { update(); });
    }
}

/*!
    \qmltype GeometryRenderer
    \instantiates Qt3DRender::QGeometryRenderer
    \inqmlmodule Qt3D.Render
    \inherits BoundingVolume
    \since 5.7
    \brief Encapsulates geometry rendering.

    A GeometryRenderer holds all the information necessary to draw
    a Geometry. A Geometry holds the coordinates of the geometry data -
    GeometryRenderer specifies how to interpret that data.
 */

/*!
    \class Qt3DRender::QGeometryRenderer
    \inmodule Qt3DRender
    \since 5.7
    \brief Encapsulates geometry rendering.

    A Qt3DRender::QGeometryRenderer holds all the information necessary to draw
    a Qt3DCore::QGeometry. A QGeometry holds the coordinates of the geometry data -
    QGeometryRenderer specifies how to interpret that data.
 */


/*!
    \enum QGeometryRenderer::PrimitiveType

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
    \qmlproperty int GeometryRenderer::instanceCount

    Holds the instance count.
 */

/*!
    \qmlproperty int GeometryRenderer::vertexCount

    Holds the vertex count.
 */

/*!
    \qmlproperty int GeometryRenderer::indexOffset

    Holds the base vertex.
 */

/*!
    \qmlproperty int GeometryRenderer::firstInstance

    Holds the base instance.
 */

/*!
    \qmlproperty int GeometryRenderer::firstVertex

    Holds the first vertex.
 */

/*!
    \qmlproperty int GeometryRenderer::indexBufferByteOffset

    Holds the byte offset into the index buffer.
 */

/*!
    \qmlproperty int GeometryRenderer::restartIndexValue

    Holds the restart index.
 */

/*!
    \qmlproperty int GeometryRenderer::verticesPerPatch

    Holds vertices per patch.
 */

/*!
    \qmlproperty bool GeometryRenderer::primitiveRestartEnabled

    Holds the primitive restart flag.
 */

/*!
    \qmlproperty Geometry GeometryRenderer::geometry

    Holds the geometry.
 */

/*!
    \qmlproperty enumeration GeometryRenderer::primitiveType

    Holds the primitive type.

    \list
    \li QGeometryRenderer.Points
    \li QGeometryRenderer.Lines
    \li QGeometryRenderer.LineLoop
    \li QGeometryRenderer.LineStrip
    \li QGeometryRenderer.Triangles
    \li QGeometryRenderer.TriangleStrip
    \li QGeometryRenderer.TriangleFan
    \li QGeometryRenderer.LinesAdjacency
    \li QGeometryRenderer.TrianglesAdjacency
    \li QGeometryRenderer.LineStripAdjacency
    \li QGeometryRenderer.TriangleStripAdjacency
    \li QGeometryRenderer.Patches
    \endlist
    \sa Qt3DRender::QGeometryRenderer::PrimitiveType
 */
/*!
    \qmlproperty float GeometryRenderer::sortIndex
    \since 6.0

    Overrides the sorting index when depth sorting is enabled.

    If depth sorting is enabled on the frame graph, the renderer will sort
    objects based on how far the center of the bounding volume is from
    the camera and render objects from the furthest to the closest.

    This property can be used to override the depth index and precisely
    control the order in which objects are rendered. This is useful when
    all objects are at the same physical distance from the camera.

    The actual values are not significant, only that they define an order
    to sort the objects. These are sorted such as the object with the
    smallest value is drawn first, then the second smallest, and so on.

    \note Setting this to -1.f will disable the explicit sorting for this
    entity and revert to using the distance from the center of the bounding
    volume.

    \sa SortPolicy
*/


/*!
    Constructs a new QGeometryRenderer with \a parent.
 */
QGeometryRenderer::QGeometryRenderer(QNode *parent)
    : Qt3DCore::QBoundingVolume(*new QGeometryRendererPrivate(), parent)
{
    Q_UNUSED(m_sortIndex)
}

/*!
    \internal
 */
QGeometryRenderer::~QGeometryRenderer()
{
}

/*!
    \internal
 */
QGeometryRenderer::QGeometryRenderer(QGeometryRendererPrivate &dd, QNode *parent)
    : Qt3DCore::QBoundingVolume(dd, parent)
{
}

/*!
    \property QGeometryRenderer::instanceCount

    Holds the instance count.
 */
int QGeometryRenderer::instanceCount() const
{
    Q_D(const QGeometryRenderer);
    return d->m_instanceCount;
}

/*!
    \property QGeometryRenderer::vertexCount

    Holds the primitive count.
 */
int QGeometryRenderer::vertexCount() const
{
    Q_D(const QGeometryRenderer);
    return d->m_vertexCount;
}

/*!
    \property QGeometryRenderer::indexOffset

    Holds the base vertex.
 */
int QGeometryRenderer::indexOffset() const
{
    Q_D(const QGeometryRenderer);
    return d->m_indexOffset;
}

/*!
    \property QGeometryRenderer::firstInstance

    Holds the base instance.
 */
int QGeometryRenderer::firstInstance() const
{
    Q_D(const QGeometryRenderer);
    return d->m_firstInstance;
}

/*!
    \property QGeometryRenderer::firstVertex

    Holds the base vertex.
 */
int QGeometryRenderer::firstVertex() const
{
    Q_D(const QGeometryRenderer);
    return d->m_firstVertex;
}

/*!
    \property QGeometryRenderer::indexBufferByteOffset

    Holds the byte offset into the index buffer.
 */
int QGeometryRenderer::indexBufferByteOffset() const
{
    Q_D(const QGeometryRenderer);
    return d->m_indexBufferByteOffset;
}

/*!
    \property QGeometryRenderer::restartIndexValue

    Holds the restart index.
 */
int QGeometryRenderer::restartIndexValue() const
{
    Q_D(const QGeometryRenderer);
    return d->m_restartIndexValue;
}

/*!
    \property QGeometryRenderer::verticesPerPatch

    Holds vertices per patch.
 */
int QGeometryRenderer::verticesPerPatch() const
{
    Q_D(const QGeometryRenderer);
    return d->m_verticesPerPatch;
}

/*!
    \property QGeometryRenderer::primitiveRestartEnabled

    Holds the primitive restart flag.
 */
bool QGeometryRenderer::primitiveRestartEnabled() const
{
    Q_D(const QGeometryRenderer);
    return d->m_primitiveRestart;
}

/*!
    \property QGeometryRenderer::geometry

    Holds the geometry.
 */
QGeometry *QGeometryRenderer::geometry() const
{
    Q_D(const QGeometryRenderer);
    return d->m_geometry;
}

/*!
    \property QGeometryRenderer::primitiveType

    Holds the primitive type.
 */
QGeometryRenderer::PrimitiveType QGeometryRenderer::primitiveType() const
{
    Q_D(const QGeometryRenderer);
    return d->m_primitiveType;
}

/*!
    \property QGeometryRenderer::sortIndex
    \since 6.0

    Overrides the sorting index when depth sorting is enabled.

    If depth sorting is enabled on the frame graph, the renderer will sort
    objects based on how far the center of the bounding volume is from
    the camera and render objects from the furthest to the closest.

    This property can be used to override the depth index and precisely
    control the order in which objects are rendered. This is useful when
    all objects are at the same physical distance from the camera.

    The actual values are not significant, only that they define an order
    to sort the objects. These are sorted such as the object with the
    smallest value is drawn first, then the second smallest, and so on.

    \note Setting this to -1.f will disable the explicit sorting for this
    entity and revert to using the distance from the center of the bounding
    volume.

    \sa Qt3DRender::QSortPolicy
*/
float QGeometryRenderer::sortIndex() const
{
    Q_D(const QGeometryRenderer);
    return d->m_sortIndex;
}

void QGeometryRenderer::setInstanceCount(int instanceCount)
{
    Q_D(QGeometryRenderer);
    if (d->m_instanceCount == instanceCount)
        return;

    d->m_instanceCount = instanceCount;
    emit instanceCountChanged(instanceCount);
}

void QGeometryRenderer::setVertexCount(int vertexCount)
{
    Q_D(QGeometryRenderer);
    if (d->m_vertexCount == vertexCount)
        return;

    d->m_vertexCount = vertexCount;
    emit vertexCountChanged(vertexCount);
}

void QGeometryRenderer::setIndexOffset(int indexOffset)
{
    Q_D(QGeometryRenderer);
    if (d->m_indexOffset == indexOffset)
        return;

    d->m_indexOffset = indexOffset;
    emit indexOffsetChanged(indexOffset);
}

void QGeometryRenderer::setFirstInstance(int firstInstance)
{
    Q_D(QGeometryRenderer);
    if (d->m_firstInstance == firstInstance)
        return;

    d->m_firstInstance = firstInstance;
    emit firstInstanceChanged(firstInstance);
}

void QGeometryRenderer::setFirstVertex(int firstVertex)
{
    Q_D(QGeometryRenderer);
    if (d->m_firstVertex == firstVertex)
        return;

    d->m_firstVertex = firstVertex;
    emit firstVertexChanged(firstVertex);
}

void QGeometryRenderer::setIndexBufferByteOffset(int offset)
{
    Q_D(QGeometryRenderer);
    if (d->m_indexBufferByteOffset == offset)
        return;

    d->m_indexBufferByteOffset = offset;
    emit indexBufferByteOffsetChanged(offset);
}

void QGeometryRenderer::setRestartIndexValue(int index)
{
    Q_D(QGeometryRenderer);
    if (index == d->m_restartIndexValue)
        return;

    d->m_restartIndexValue = index;
    emit restartIndexValueChanged(index);
}

void QGeometryRenderer::setVerticesPerPatch(int verticesPerPatch)
{
    Q_D(QGeometryRenderer);
    if (d->m_verticesPerPatch != verticesPerPatch) {
        d->m_verticesPerPatch = verticesPerPatch;
        emit verticesPerPatchChanged(verticesPerPatch);
    }
}

void QGeometryRenderer::setPrimitiveRestartEnabled(bool enabled)
{
    Q_D(QGeometryRenderer);
    if (enabled == d->m_primitiveRestart)
        return;

    d->m_primitiveRestart = enabled;
    emit primitiveRestartEnabledChanged(enabled);
}

void QGeometryRenderer::setGeometry(QGeometry *geometry)
{
    Q_D(QGeometryRenderer);
    if (d->m_geometry == geometry)
        return;

    if (d->m_geometry)
        d->unregisterDestructionHelper(d->m_geometry);

    if (geometry && !geometry->parent())
        geometry->setParent(this);

    d->m_geometry = geometry;

    // Ensures proper bookkeeping
    if (d->m_geometry)
        d->registerDestructionHelper(d->m_geometry, &QGeometryRenderer::setGeometry, d->m_geometry);

    emit geometryChanged(geometry);
}

void QGeometryRenderer::setPrimitiveType(QGeometryRenderer::PrimitiveType primitiveType)
{
    Q_D(QGeometryRenderer);
    if (d->m_primitiveType == primitiveType)
        return;

    d->m_primitiveType = primitiveType;
    emit primitiveTypeChanged(primitiveType);
}

void QGeometryRenderer::setSortIndex(float sortIndex)
{
    Q_D(QGeometryRenderer);
    if (qFuzzyCompare(d->m_sortIndex, sortIndex))
        return;

    d->m_sortIndex = sortIndex;
    emit sortIndexChanged(d->m_sortIndex);
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qgeometryrenderer.cpp"
