// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmemorybarrier.h"
#include "qmemorybarrier_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QMemoryBarrier
    \inmodule Qt3DRender
    \since 5.9
    \ingroup framegraph
    \brief Class to emplace a memory barrier.

    A Qt3DRender::QMemoryBarrier FrameGraph node is used to emplace a specific
    memory barrier at a specific time of the rendering. This is required to
    properly synchronize drawing and compute commands on the GPU.

    The barrier defines the ordering of memory operations issued by a prior
    command. This means that if command1 is manipulating a buffer that is to be
    used as a vertex attribute buffer in a following command2, then the memory
    barrier should be placed after command1 and setting the appropriate barrier
    type for vertex attribute buffer.

    When a QMemoryBarrier node is found in a FrameGraph branch, the barrier
    will be enforced prior to any draw or compute command even if these are
    defined deeper in the branch.

    For OpenGL rendering, this page gives more info about the
    \l {https://www.opengl.org/wiki/Memory_Model}{Memory Model}
 */

/*!
    \qmltype MemoryBarrier
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QMemoryBarrier
    \inherits FrameGraphNode
    \since 5.9
    \brief Class to place a memory barrier.

    A MemoryBarrier FrameGraph node is used to emplace a specific
    memory barrier at a specific time of the rendering. This is required to
    properly synchronize drawing and compute commands on the GPU.

    The barrier defines the ordering of memory operations issued by a prior
    command. This means that if command1 is manipulating a buffer that is to be
    used as a vertex attribute buffer in a following command2, then the memory
    barrier should be placed after command1 and setting the appropriate barrier
    type for vertex attribute buffer.

    When a QMemoryBarrier node is found in a FrameGraph branch, the barrier
    will be enforced prior to any draw or compute command even if these are
    defined deeper in the branch.

    For OpenGL rendering, this page gives more info about the
    \l {https://www.opengl.org/wiki/Memory_Model}{Memory Model}
*/

/*!
    \enum QMemoryBarrier::Operation

    This enum type describes types of buffer to be cleared.
    \value None
    \value ElementArray
    \value Uniform
    \value TextureFetch
    \value ShaderImageAccess
    \value Command
    \value PixelBuffer
    \value TextureUpdate
    \value BufferUpdate
    \value FrameBuffer
    \value TransformFeedback
    \value AtomicCounter
    \value ShaderStorage
    \value QueryBuffer
    \value VertexAttributeArray
    \value All
*/


QMemoryBarrierPrivate::QMemoryBarrierPrivate()
    : QFrameGraphNodePrivate()
    , m_waitOperations(QMemoryBarrier::None)
{
}

QMemoryBarrier::QMemoryBarrier(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QMemoryBarrierPrivate(), parent)
{
}

QMemoryBarrier::~QMemoryBarrier()
{
}

void QMemoryBarrier::setWaitOperations(QMemoryBarrier::Operations waitOperations)
{
    Q_D(QMemoryBarrier);
    if (waitOperations != d->m_waitOperations) {
        d->m_waitOperations = waitOperations;
        emit waitOperationsChanged(waitOperations);
        d->notifyPropertyChange("waitOperations", QVariant::fromValue(waitOperations));  // TODOSYNC
    }
}

QMemoryBarrier::Operations QMemoryBarrier::waitOperations() const
{
    Q_D(const QMemoryBarrier);
    return d->m_waitOperations;
}

QMemoryBarrier::QMemoryBarrier(QMemoryBarrierPrivate &dd, Qt3DCore::QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qmemorybarrier.cpp"
