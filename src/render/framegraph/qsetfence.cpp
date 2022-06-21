// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsetfence.h"
#include "qsetfence_p.h"

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

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qsetfence.cpp"
