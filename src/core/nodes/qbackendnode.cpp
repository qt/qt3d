// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qbackendnode.h"
#include "qbackendnode_p.h"

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qnode.h>

#include <Qt3DCore/private/corelogging_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*! \internal */
QBackendNodeMapper::~QBackendNodeMapper()
{
}

QBackendNodePrivate::QBackendNodePrivate(QBackendNode::Mode mode)
    : q_ptr(nullptr)
    , m_mode(mode)
    , m_enabled(false)
{
}

QBackendNodePrivate::~QBackendNodePrivate() = default;

void QBackendNodePrivate::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

QBackendNodePrivate *QBackendNodePrivate::get(QBackendNode *n)
{
    return n->d_func();
}

void QBackendNodePrivate::addedToEntity(QNode *frontend)
{
    Q_UNUSED(frontend);
}

void QBackendNodePrivate::removedFromEntity(QNode *frontend)
{
    Q_UNUSED(frontend);
}

void QBackendNodePrivate::componentAdded(QNode *frontend)
{
    Q_UNUSED(frontend);
}

void QBackendNodePrivate::componentRemoved(QNode *frontend)
{
    Q_UNUSED(frontend);
}

/*!
 * \class Qt3DCore::QBackendNodeMapper
 * \inheaderfile Qt3DCore/QBackendNodeMapper
 * \inmodule Qt3DCore
 *
 * \brief Creates and maps backend nodes to their respective frontend nodes.
 */

/*!
 * \fn Qt3DCore::QBackendNode *Qt3DCore::QBackendNodeMapper::create(Qt3DCore::QNodeId id) const
 *
 * \TODO
 *
 * \a id
 *
 * \return created node.
 */

/*!
 * \fn Qt3DCore::QBackendNode * Qt3DCore::QBackendNodeMapper::get(Qt3DCore::QNodeId id) const
 *
 * \return backend node for the given node \a id.
 */

/*!
 * \fn void Qt3DCore::QBackendNodeMapper::destroy(Qt3DCore::QNodeId id) const
 *
 * Destroys the backend node for the given node \a id.
 */

/*!
 * \class Qt3DCore::QBackendNode
 * \inheaderfile Qt3DCore/QBackendNode
 * \inmodule Qt3DCore
 *
 * \brief The base class for all Qt3D backend nodes.
 */

/*!
 * \enum Qt3DCore::QBackendNode::Mode
 *
 * The mode for the backend node.
 *
 * \value ReadOnly
 * \value ReadWrite
 */

QBackendNode::QBackendNode(QBackendNode::Mode mode)
    : d_ptr(new QBackendNodePrivate(mode))
{
    d_ptr->q_ptr = this;
}

QBackendNode::~QBackendNode()
{
    delete d_ptr;
}

/*!
 * Sets the peer \a id.
 */
void QBackendNode::setPeerId(QNodeId id) noexcept
{
    Q_D(QBackendNode);
    d->m_peerId = id;
}

/*!
 * \return the peer id of the backend node.
 */
QNodeId QBackendNode::peerId() const noexcept
{
    Q_D(const QBackendNode);
    return d->m_peerId;
}

/*!
 * \return \c true if the backend node is enabled.
 */
bool QBackendNode::isEnabled() const noexcept
{
    Q_D(const QBackendNode);
    return d->m_enabled;
}

/*!
 * \return the mode of the backend mode.
 */
QBackendNode::Mode QBackendNode::mode() const noexcept
{
    Q_D(const QBackendNode);
    return d->m_mode;
}

/*!
 * \brief QBackendNode::syncFromFrontEnd
 * \param frontEnd
 * \param firstTime
 *
 * This is called by the aspect when a \a frontEnd node needs to synchronize its changes
 * with the backend (normally due to property changes).
 *
 * \a firstTime will be true if the backend node was just created
 */
void QBackendNode::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    Q_UNUSED(frontEnd);
    Q_UNUSED(firstTime);
}

/*!
 * \internal
 */
QBackendNode::QBackendNode(QBackendNodePrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
 * Enables or disables the backend node by \a enabled.
 */
void QBackendNode::setEnabled(bool enabled) noexcept
{
    Q_D(QBackendNode);
    d->m_enabled = enabled;
}

} // Qt3D

QT_END_NAMESPACE
