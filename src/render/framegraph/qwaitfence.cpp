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

#include "qwaitfence.h"
#include "qwaitfence_p.h"
#include <Qt3DRender/private/qframegraphnode_p.h>
#include <Qt3DRender/private/qframegraphnodecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QWaitFencePrivate::QWaitFencePrivate()
    : QFrameGraphNodePrivate()
    , m_handleType(QWaitFence::NoHandle)
    , m_handle(QVariant())
    , m_waitOnCPU(false)
    , m_timeout(std::numeric_limits<quint64>::max())
{
}

/*!
    \class Qt3DRender::QWaitFence
    \inmodule Qt3DRender
    \brief FrameGraphNode used to wait for a fence in the graphics command
    stream to become signaled.

    Fence allow to synchronosize GPU and CPU workloads. GPU commands usually
    are non-blocking. When issued, commands are inserted in command buffers
    which will be read at a later time by the GPU. In some cases, you want to
    continue processing or issue specific command only when you are sure a
    command has been executed by the hardware. Fences are a way to do so. This
    is especially important when using 3rd party engines with Qt3D, Qt3D should
    only access shared resources when we know the other engine command are done
    modifying the resource.

    QWaitFence is a FrameGraph node that will force to wait for it to become
    signaled before subsequent commands are inserted into the command stream.
    It can then be used in conjunction with \l QSetFence and contains
    properties to configure how long it should wait and whether it should block
    on the CPU side.

    \note Qt 3D uploads GPU resources (Texture, Shaders, Buffers) before
    issuing draw calls.

    \since 5.13
 */

QWaitFence::QWaitFence(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QWaitFencePrivate(), parent)
{
}

QWaitFence::~QWaitFence()
{
}

/*!
    \qmlproperty bool WaitFence::waitOnCPU

    Specifies whether the CPU should be block while waiting for the fence to
    become signaled. This is false by default.
*/
/*!
    \property Qt3DRender::QWaitFence::waitOnCPU

    Specifies whether the CPU should be block while waiting for the fence to
    become signaled. This is false by default.
*/
bool QWaitFence::waitOnCPU() const
{
    Q_D(const QWaitFence);
    return d->m_waitOnCPU;
}

void QWaitFence::setWaitOnCPU(bool waitOnCPU)
{
    Q_D(QWaitFence);
    if (d->m_waitOnCPU == waitOnCPU)
        return;
    d->m_waitOnCPU = waitOnCPU;
    emit waitOnCPUChanged(waitOnCPU);
}

/*!
    \qmlproperty int WaitFence::timeout

    Specifies the maximum amount of time in nanoseconds to wait for the fence
    to become signaled.
*/
/*!
    \property Qt3DRender::QWaitFence::timeout

    Specifies the maximum amount of time in nanoseconds to wait for the fence
    to become signaled.
*/
quint64 QWaitFence::timeout() const
{
    Q_D(const QWaitFence);
    return d->m_timeout;
}

void QWaitFence::setTimeout(quint64 timeout)
{
    Q_D(QWaitFence);
    if (d->m_timeout == timeout)
        return;
    d->m_timeout = timeout;
    emit timeoutChanged(timeout);
}

QWaitFence::QWaitFence(QWaitFencePrivate &dd, Qt3DCore::QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

/*!
    \qmlproperty HandleType WaitFence::handleType

    Specifies the type of handle being used. Currently only OpenGL Fence ids
    are supported.
*/
/*!
    \property Qt3DRender::QWaitFence::handleType

    Specifies the type of handle being used. Currently only OpenGL Fence ids
    are supported.
*/
QWaitFence::HandleType QWaitFence::handleType() const
{
    Q_D(const QWaitFence);
    return d->m_handleType;
}

void QWaitFence::setHandleType(QWaitFence::HandleType type)
{
    Q_D(QWaitFence);
    if (d->m_handleType != type) {
        d->m_handleType = type;
        emit handleTypeChanged(type);
    }
}

/*!
    \qmlproperty variant WaitFence::handle

    Holds the underlying fence handle wrapped in a variant.
*/
/*!
    \property Qt3DRender::QWaitFence::handle

    Holds the underlying fence handle wrapped in a QVariant.
*/
QVariant QWaitFence::handle() const
{
    Q_D(const QWaitFence);
    return d->m_handle;
}

void QWaitFence::setHandle(QVariant handle)
{
    Q_D(QWaitFence);
    if (d->m_handle != handle) {
        d->m_handle = handle;
        emit handleChanged(handle);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QWaitFence::createNodeCreationChange() const
{
    auto creationChange = QFrameGraphNodeCreatedChangePtr<QWaitFenceData>::create(this);
    QWaitFenceData &data = creationChange->data;
    Q_D(const QWaitFence);
    data.handleType = d->m_handleType;
    data.handle = d->m_handle;
    data.timeout = d->m_timeout;
    data.waitOnCPU = d->m_waitOnCPU;
    return creationChange;
}

} // Qt3DRender

QT_END_NAMESPACE
