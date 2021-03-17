/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsetfence.h"
#include "qsetfence_p.h"
#include <Qt3DRender/private/qframegraphnodecreatedchange_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QSetFencePrivate::QSetFencePrivate()
    : QFrameGraphNodePrivate()
    , m_handleType(QSetFence::NoHandle)
    , m_handle(QVariant())
{
}

/*!
    \class Qt3DRender::QSetFence
    \inmodule Qt3DRender
    \brief FrameGraphNode used to insert a fence in the graphics command stream.

    Fence allow to synchronosize GPU and CPU workloads. GPU commands usually
    are non-blocking. When issued, commands are inserted in command buffers
    which will be read at a later time by the GPU. In some cases, you want to
    continue processing or issue specific command only when you are sure a
    command has been executed by the hardware. Fences are a way to do so. This
    is especially important when using 3rd party engines with Qt3D, Qt3D should
    only access shared resources when we know the other engine command are done
    modifying the resource.

    QSetFence is a FrameGraph node that inserts a fence into the command
    stream. It can then be used in conjunction with \l QWaitFence or by
    extracting the underlying handle.

    The handle property will be updated once the renderer has created the
    underlying fence resource. The handle will remain valid as long as it
    remains in the unsignaled state. Once it has reached the signaled state, it
    will be destroyed and a new handle will be created. That means that
    depending on how long it takes for the fence to be signaled, the same
    handle could be used over several frames.

    \since 5.13
 */
QSetFence::QSetFence(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QSetFencePrivate(), parent)
{
}

QSetFence::~QSetFence()
{
}

QSetFence::QSetFence(QSetFencePrivate &dd, Qt3DCore::QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

/*!
    \qmlproperty HandleType SetFence::handleType

    Specifies the type of handle being used. Currently only OpenGL Fence ids
    are supported.
*/
/*!
    \property Qt3DRender::QSetFence::handleType

    Specifies the type of handle being used. Currently only OpenGL Fence ids
    are supported.
*/
QSetFence::HandleType QSetFence::handleType() const
{
    Q_D(const QSetFence);
    return d->m_handleType;
}

void QSetFencePrivate::setHandleType(QSetFence::HandleType type)
{
    Q_Q(QSetFence);
    if (m_handleType != type) {
        const bool blocked = q->blockNotifications(true);
        m_handleType = type;
        emit q->handleTypeChanged(type);
        q->blockNotifications(blocked);
    }
}

/*!
    \qmlproperty variant SetFence::handle

    Holds the underlying fence handle wrapped in a variant.
*/
/*!
    \property Qt3DRender::QSetFence::handle

    Holds the underlying fence handle wrapped in a QVariant.
*/
QVariant QSetFence::handle() const
{
    Q_D(const QSetFence);
    return d->m_handle;
}

void QSetFencePrivate::setHandle(QVariant handle)
{
    Q_Q(QSetFence);
    if (m_handle != handle) {
        const bool blocked = q->blockNotifications(true);
        m_handle = handle;
        emit q->handleChanged(handle);
        q->blockNotifications(blocked);
    }
}

void QSetFence::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change)
{
    Qt3DCore::QPropertyUpdatedChangePtr e = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(change);
    if (e->type() == Qt3DCore::PropertyUpdated) {
        Q_D(QSetFence);
        if (e->propertyName() == QByteArrayLiteral("handle"))
            d->setHandle(e->value());
        else if (e->propertyName() == QByteArrayLiteral("handleType"))
            d->setHandleType(static_cast<Qt3DRender::QSetFence::HandleType>(e->value().toInt()));
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QSetFence::createNodeCreationChange() const
{
    auto creationChange = QFrameGraphNodeCreatedChangePtr<QSetFenceData>::create(this);
    QSetFenceData &data = creationChange->data;
    Q_UNUSED(data); // Might be of use later
    return creationChange;
}

} // Qt3DRender

QT_END_NAMESPACE
