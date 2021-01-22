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

#include "qnodedestroyedchange.h"
#include "qnodedestroyedchange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*!
 * \class Qt3DCore::QNodeDestroyedChange
 * \inheaderfile Qt3DCore/QNodeDestroyedChange
 * \inherits Qt3DCore::QSceneChange
 * \since 5.7
 * \inmodule Qt3DCore
 * \brief Used to notify when a node is destroyed.
 */

/*!
 * \typedef Qt3DCore::QNodeDestroyedChangePtr
 * \relates Qt3DCore::QNodeDestroyedChange
 *
 * A shared pointer for QNodeDestroyedChange.
 */

/*!
 * Constructs a new QNodeDestroyedChange with \a node and its \a subtreeIdsAndTypes.
 */
QNodeDestroyedChange::QNodeDestroyedChange(const QNode *node, const QVector<QNodeIdTypePair> &subtreeIdsAndTypes)
    : QSceneChange(*new QNodeDestroyedChangePrivate, NodeDeleted, node->id())
{
    Q_D(QNodeDestroyedChange);
    d->m_subtreeIdsAndTypes = subtreeIdsAndTypes;
}

QNodeDestroyedChange::~QNodeDestroyedChange()
{
}

/*!
   \return a vector of subtree node id type pairs
 */
QVector<QNodeIdTypePair> QNodeDestroyedChange::subtreeIdsAndTypes() const
{
    Q_D(const QNodeDestroyedChange);
    return d->m_subtreeIdsAndTypes;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
