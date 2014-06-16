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

#include "cameramanager.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

CameraManager::CameraManager() :
    QAbstractResourcesManager<RenderCamera, QUuid, 8>()
{
}

/*! Returns true if there is a RenderCamera associated to the entity
 *  QUuid \a id. False otherwise.
 * \fn bool CameraManager::hasCamera(const QUuid &id) const
 */

/*!
 * Returns the camera associated to the Entity with QUuid \a id.
 * If no camera has already been associated to \a id, one is implicitely
 * created.
 * Note that the Q_NULLPTR can be returned if for some reason the
 * camera has been released internally. This could indicate a
 * programming error somewhere.
 * \fn RenderCamera *CameraManager::getOrCreateRenderCamera(const QUuid &id)
 */

/*!
 * Returns the camera associated to the Entity defined by \a id.
 * Q_NULLPTR is returned if no camera is associated.
 * \fn RenderCamera *CameraManager::renderCamera(const QUuid &id)
 */

/*!
 * Releases the RenderCamera associated with \a id.
 * \fn void CameraManager::releaseRenderCamera(const QUuid &id)
 */

} // Render

} // Qt3D

QT_END_NAMESPACE
