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

#include "entitymanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

EntityManager::EntityManager() :
    QResourcesManager<RenderEntity, QUuid, 16>()
{
}

/*!
 * Returns true if there is a RenderNode associated to the Entity
 * identified by \a id. Returns false otherwise.
 * \fn bool RenderNodesManager::hasRenderNode(const QUuid &id) const
 */

/*!
 * Returns a RenderNode from an entity \a id. The RenderNode is
 * created if needed but Q_NULLPTR can be returned if internally
 * the RenderNode has been released.
 * \fn RenderNode *RenderNodesManager::getOrCreateRenderNode(const QUuid &id)
 */

/*!
 * Returns the RenderNode associated with the Entity of id \a id.
 * Q_NULLPTR if none could be found.
 * \fn RenderNode *RenderNodesManager::renderNode(const QUuid &id)
 */

/*!
 * Releases the RenderNode resource associated to the Entity of id \a id.
 * \fn void RenderNodesManager::releaseRenderNode(const QUuid &id)
 */

} // Render

} // Qt3D

QT_END_NAMESPACE
