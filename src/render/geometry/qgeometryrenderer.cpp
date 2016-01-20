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

#include "qgeometryrenderer.h"
#include "qgeometryrenderer_p.h"

#include <private/qcomponent_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QGeometryRendererPrivate::QGeometryRendererPrivate()
    : QComponentPrivate()
    , m_instanceCount(1)
    , m_primitiveCount(0)
    , m_baseVertex(0)
    , m_baseInstance(0)
    , m_restartIndex(-1)
    , m_primitiveRestart(false)
    , m_geometry(Q_NULLPTR)
    , m_primitiveType(QGeometryRenderer::Triangles)
{
}

/*!
 * \qmltype GeometryRenderer
 * \instantiates Qt3DRender::QGeometryRenderer
 * \inqmlmodule Qt3D.Render
 */

/*!
 * \qmlproperty int GeometryRenderer::instanceCount
 *
 * Holds the instance count.
 */

/*!
 * \qmlproperty int GeometryRenderer::primitiveCount
 *
 * Holds the primitive count.
 */

/*!
 * \qmlproperty int GeometryRenderer::baseVertex
 *
 * Holds the base vertex.
 */

/*!
 * \qmlproperty int GeometryRenderer::baseInstance
 *
 * Holds the base instance.
 */

/*!
 * \qmlproperty int GeometryRenderer::restartIndex
 *
 * Holds the restart index.
 */

/*!
 * \qmlproperty bool GeometryRenderer::primitiveRestart
 *
 * Holds the primitive restart flag.
 */

/*!
 * \qmlproperty Geometry GeometryRenderer::geometry
 *
 * Holds the geometry.
 */

/*!
 * \qmlproperty QGeometryRenderer::PrimitiveType GeometryRenderer::primitiveType
 *
 * Holds the primitive type.
 */

/*!
 * \class Qt3DRender::QGeometryRenderer
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DCore::QComponent
 *
 */

/*!
 * \enum QGeometryRenderer::PrimitiveType
 *
 * The type of the primitive.
 *
 * \value Points
 * \value Lines
 * \value LineLoop
 * \value LineStrip
 * \value Triangles
 * \value TriangleStrip
 * \value TriangleFan
 * \value LinesAdjacency
 * \value TrianglesAdjacency
 * \value LineStripAdjacency
 * \value TriangleStripAdjacency
 * \value Patches
 */

/*!
 * Constructs a new QGeometryRenderer with \a parent.
 */
QGeometryRenderer::QGeometryRenderer(QNode *parent)
    : QComponent(*new QGeometryRendererPrivate(), parent)
{
}

/*!
 * Destroys this geometry renderer.
 */
QGeometryRenderer::~QGeometryRenderer()
{
    QComponent::cleanup();
}

/*!
 * \internal
 */
QGeometryRenderer::QGeometryRenderer(QGeometryRendererPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
 * \property QGeometryRenderer::instanceCount
 *
 * Holds the instance count.
 */
int QGeometryRenderer::instanceCount() const
{
    Q_D(const QGeometryRenderer);
    return d->m_instanceCount;
}

/*!
 * \property QGeometryRenderer::primitiveCount
 *
 * Holds the primitive count.
 */
int QGeometryRenderer::primitiveCount() const
{
    Q_D(const QGeometryRenderer);
    return d->m_primitiveCount;
}

/*!
 * \property QGeometryRenderer::baseVertex
 *
 * Holds the base vertex.
 */
int QGeometryRenderer::baseVertex() const
{
    Q_D(const QGeometryRenderer);
    return d->m_baseVertex;
}

/*!
 * \property QGeometryRenderer::baseInstance
 *
 * Holds the base instance.
 */
int QGeometryRenderer::baseInstance() const
{
    Q_D(const QGeometryRenderer);
    return d->m_baseInstance;
}

/*!
 * \property QGeometryRenderer::restartIndex
 *
 * Holds the restart index.
 */
int QGeometryRenderer::restartIndex() const
{
    Q_D(const QGeometryRenderer);
    return d->m_restartIndex;
}

/*!
 * \property QGeometryRenderer::primitiveRestart
 *
 * Holds the primitive restart flag.
 */
bool QGeometryRenderer::primitiveRestart() const
{
    Q_D(const QGeometryRenderer);
    return d->m_primitiveRestart;
}

/*!
 * \property QGeometryRenderer::geometry
 *
 * Holds the geometry.
 */
QGeometry *QGeometryRenderer::geometry() const
{
    Q_D(const QGeometryRenderer);
    return d->m_geometry;
}

/*!
 * \property QGeometryRenderer::primitiveType
 *
 * Holds the primitive type.
 */
QGeometryRenderer::PrimitiveType QGeometryRenderer::primitiveType() const
{
    Q_D(const QGeometryRenderer);
    return d->m_primitiveType;
}

/*!
 * \return the geometry functor.
 */
QGeometryFunctorPtr QGeometryRenderer::geometryFunctor() const
{
    Q_D(const QGeometryRenderer);
    return d->m_functor;
}

void QGeometryRenderer::setInstanceCount(int instanceCount)
{
    Q_D(QGeometryRenderer);
    if (d->m_instanceCount == instanceCount)
        return;

    d->m_instanceCount = instanceCount;
    emit instanceCountChanged(instanceCount);
}

void QGeometryRenderer::setPrimitiveCount(int primitiveCount)
{
    Q_D(QGeometryRenderer);
    if (d->m_primitiveCount == primitiveCount)
        return;

    d->m_primitiveCount = primitiveCount;
    emit primitiveCountChanged(primitiveCount);
}

void QGeometryRenderer::setBaseVertex(int baseVertex)
{
    Q_D(QGeometryRenderer);
    if (d->m_baseVertex == baseVertex)
        return;

    d->m_baseVertex = baseVertex;
    emit baseVertexChanged(baseVertex);
}

void QGeometryRenderer::setBaseInstance(int baseInstance)
{
    Q_D(QGeometryRenderer);
    if (d->m_baseInstance == baseInstance)
        return;

    d->m_baseInstance = baseInstance;
    emit baseInstanceChanged(baseInstance);
}

void QGeometryRenderer::setRestartIndex(int index)
{
    Q_D(QGeometryRenderer);
    if (index == d->m_restartIndex)
        return;

    d->m_restartIndex = index;
    emit restartIndexChanged(index);
}

void QGeometryRenderer::setPrimitiveRestart(bool enabled)
{
    Q_D(QGeometryRenderer);
    if (enabled == d->m_primitiveRestart)
        return;

    d->m_primitiveRestart = enabled;
    emit primitiveRestartChanged(enabled);
}

void QGeometryRenderer::setGeometry(QGeometry *geometry)
{
    Q_D(QGeometryRenderer);
    if (d->m_geometry == geometry)
        return;

    if (d->m_geometry && d->m_changeArbiter) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("geometry");
        change->setValue(QVariant::fromValue(d->m_geometry->id()));
        d->notifyObservers(change);
    }

    if (geometry && !geometry->parent())
        geometry->setParent(this);

    d->m_geometry = geometry;
    const bool blocked = blockNotifications(true);
    emit geometryChanged(geometry);
    blockNotifications(blocked);

    if (d->m_geometry && d->m_changeArbiter) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
        change->setPropertyName("geometry");
        change->setValue(QVariant::fromValue(d->m_geometry->id()));
        d->notifyObservers(change);
    }
}

void QGeometryRenderer::setPrimitiveType(QGeometryRenderer::PrimitiveType primitiveType)
{
    Q_D(QGeometryRenderer);
    if (d->m_primitiveType == primitiveType)
        return;

    d->m_primitiveType = primitiveType;
    emit primitiveTypeChanged(primitiveType);
}

/*!
 * Sets the geometry \a functor.
 */
void QGeometryRenderer::setGeometryFunctor(const QGeometryFunctorPtr &functor)
{
    Q_D(QGeometryRenderer);
    if (functor && d->m_functor && *functor == *d->m_functor)
        return;
    d->m_functor = functor;
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
        change->setPropertyName("geometryFunctor");
        change->setValue(QVariant::fromValue(d->m_functor));
        d->notifyObservers(change);
    }
}

/*!
 * \internal
 */
void QGeometryRenderer::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QGeometryRenderer *other = static_cast<const QGeometryRenderer *>(ref);
    d_func()->m_instanceCount = other->d_func()->m_instanceCount;
    d_func()->m_primitiveCount = other->d_func()->m_primitiveCount;
    d_func()->m_baseVertex = other->d_func()->m_baseVertex;
    d_func()->m_baseInstance = other->d_func()->m_baseInstance;
    d_func()->m_restartIndex = other->d_func()->m_restartIndex;
    d_func()->m_primitiveRestart = other->d_func()->m_primitiveRestart;
    d_func()->m_primitiveType = other->d_func()->m_primitiveType;
    d_func()->m_geometry = static_cast<QGeometry *>(QNode::clone(other->d_func()->m_geometry));
    d_func()->m_functor = other->d_func()->m_functor;
}

/*!
 * \internal
 */
void QGeometryRenderer::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    Q_D(QGeometryRenderer);
    QScenePropertyChangePtr change = qSharedPointerCast<QScenePropertyChange>(e);
    if (change->type() == NodeUpdated && change->propertyName() == QByteArrayLiteral("geometry")) {
        QNodePtr nodePtr = change->value().value<QNodePtr>();
        QGeometry *backendGeometry = static_cast<QGeometry *>(nodePtr.data());
        QGeometry *frontendGeometry = static_cast<QGeometry *>(QNode::clone(backendGeometry));
        if (frontendGeometry != Q_NULLPTR)
            d->insertTree(frontendGeometry);
        setGeometry(frontendGeometry);
    }
}



} // namespace Qt3DRender

QT_END_NAMESPACE
