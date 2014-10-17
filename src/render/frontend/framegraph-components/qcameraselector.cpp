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

#include "qcameraselector.h"
#include "qcameraselector_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QCameraSelector::QCameraSelector(QCameraSelectorPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QCameraSelectorPrivate::QCameraSelectorPrivate(Qt3D::QCameraSelector *qq)
    : QFrameGraphNodePrivate(qq)
    , m_camera(Q_NULLPTR)
{}

void QCameraSelector::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QCameraSelector *other = static_cast<const QCameraSelector*>(ref);
    Q_FOREACH (QFrameGraphNode *fgChild, other->d_func()->m_fgChildren)
        appendFrameGraphNode(qobject_cast<QFrameGraphNode *>(QNodePrivate::get(fgChild)->clone()));

    if (other->d_func()->m_camera)
        setCamera(qobject_cast<QEntity *>(QNodePrivate::get(other->d_func()->m_camera)->clone()));
}

QCameraSelector::QCameraSelector(Qt3D::QNode *parent)
    :   QFrameGraphNode(*new QCameraSelectorPrivate(this), parent)
{}

void QCameraSelector::setCamera(QEntity *camera)
{
    Q_D(QCameraSelector);
    if (d->m_camera != camera) {
        d->m_camera = camera;

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!camera->parent())
            camera->setParent(this);
        emit cameraChanged();
    }
}

QEntity *QCameraSelector::camera() const
{
    Q_D(const QCameraSelector);
    return d->m_camera;
}

} // Qt3D

QT_END_NAMESPACE
