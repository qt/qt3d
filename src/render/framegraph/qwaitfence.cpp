// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qwaitfence.h"
#include "qwaitfence_p.h"
#include <Qt3DRender/private/qframegraphnode_p.h>

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

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qwaitfence.cpp"
