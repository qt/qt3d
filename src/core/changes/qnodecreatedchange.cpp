/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qnodecreatedchange.h"
#include "qnodecreatedchange_p.h"

#include <Qt3DCore/qnode.h>

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QNodeCreatedChangeBasePrivate::QNodeCreatedChangeBasePrivate(const QNode *node)
    : QSceneChangePrivate()
    , m_parentId(node->parentNode() ? node->parentNode()->id() : QNodeId())
    , m_metaObject(QNodePrivate::findStaticMetaObject(node->metaObject()))
    , m_nodeEnabled(node->isEnabled())
{
}

/*!
 * \class Qt3DCore::QNodeCreatedChangeBase
 * \inheaderfile Qt3DCore/QNodeCreatedChangeBase
 * \inherits Qt3DCore::QSceneChange
 * \inmodule Qt3DCore
 * \brief The QNodeCreatedChangeBase class is the base class for all NodeCreated QSceneChange events.
 *
 * The QNodeCreatedChangeBase class is the base class for all QSceneChange events that
 * have the changeType() NodeCreated. You should not need to instantiate this class.
 * Usually you should be using one of its subclasses such as QNodeCreatedChange.
 *
 * You can subclass this to create your own node update types for communication between
 * your QNode and QBackendNode subclasses when writing your own aspects.
 */

/*!
 * \typedef Qt3DCore::QNodeCreatedChangeBasePtr
 * \relates Qt3DCore::QNodeCreatedChangeBase
 *
 * A shared pointer for QNodeCreatedChangeBase.
 */

/*!
 * Constructs a new QNodeCreatedChangeBase with \a node.
 */
QNodeCreatedChangeBase::QNodeCreatedChangeBase(const QNode *node)
    : QSceneChange(*new QNodeCreatedChangeBasePrivate(node), NodeCreated, node->id())
{
}

QNodeCreatedChangeBase::QNodeCreatedChangeBase(QNodeCreatedChangeBasePrivate &dd, const QNode *node)
    : QSceneChange(dd, NodeCreated, node->id())
{
}

QNodeCreatedChangeBase::~QNodeCreatedChangeBase()
{
}

/*!
 * \return parent id.
 */
QNodeId QNodeCreatedChangeBase::parentId() const Q_DECL_NOTHROW
{
    Q_D(const QNodeCreatedChangeBase);
    return d->m_parentId;
}

/*!
 * \return metaobject.
 */
const QMetaObject *QNodeCreatedChangeBase::metaObject() const Q_DECL_NOTHROW
{
    Q_D(const QNodeCreatedChangeBase);
    return d->m_metaObject;
}

/*!
 * \return node enabled.
 */
bool QNodeCreatedChangeBase::isNodeEnabled() const Q_DECL_NOTHROW
{
    Q_D(const QNodeCreatedChangeBase);
    return d->m_nodeEnabled;
}

} // namespace Qt3DCore

/*!
 * \class Qt3DCore::QNodeCreatedChange
 * \inheaderfile Qt3DCore/QNodeCreatedChange
 * \inherits Qt3DCore::QNodeCreatedChangeBase
 * \since 5.7
 * \inmodule Qt3DCore
 * \brief Used to notify when a node is created.
 */

QT_END_NAMESPACE
