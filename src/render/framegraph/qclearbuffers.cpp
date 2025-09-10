// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qclearbuffers.h"
#include "qclearbuffers_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
/*!
    \class Qt3DRender::QClearBuffers
    \inmodule Qt3DRender
    \since 5.7
    \ingroup framegraph
    \brief Class to clear buffers.

    A Qt3DRender::QClearBuffers FrameGraph node enables clearing of the specific
    render target buffers with specific values.
 */

/*!
    \qmltype ClearBuffers
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QClearBuffers
    \inherits FrameGraphNode
    \since 5.7
    \brief Class to clear buffers.

    A Qt3DRender::QClearBuffers FrameGraph node enables clearing of the specific
    render target buffers with specific values.
*/

/*!
    \enum Qt3DRender::QClearBuffers::BufferType

    This enum type describes types of buffer to be cleared.
    \value None No buffers will be cleared
    \value ColorBuffer Clear color buffers
    \value DepthBuffer Clear depth buffer
    \value StencilBuffer Clear stencil buffer
    \value DepthStencilBuffer Clear depth and stencil buffers
    \value ColorDepthBuffer Clear color and depth buffers
    \value ColorDepthStencilBuffer Clear color, depth and stencil buffers
    \value AllBuffers Clear all buffers
*/

QClearBuffersPrivate::QClearBuffersPrivate()
    : QFrameGraphNodePrivate()
    , m_buffersType(QClearBuffers::None)
    , m_clearDepthValue(1.f)
    , m_clearStencilValue(0)
    , m_buffer(nullptr)
{
}

/*!
    The constructor creates an instance with the specified \a parent.
 */
QClearBuffers::QClearBuffers(QNode *parent)
    : QFrameGraphNode(*new QClearBuffersPrivate, parent)
{
}

/*! \internal */
QClearBuffers::~QClearBuffers()
{
}

/*! \internal */
QClearBuffers::QClearBuffers(QClearBuffersPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}


QClearBuffers::BufferType QClearBuffers::buffers() const
{
    Q_D(const QClearBuffers);
    return d->m_buffersType;
}

QColor QClearBuffers::clearColor() const
{
    Q_D(const QClearBuffers);
    return d->m_clearColor;
}

float QClearBuffers::clearDepthValue() const
{
    Q_D(const QClearBuffers);
    return d->m_clearDepthValue;
}

int QClearBuffers::clearStencilValue() const
{
    Q_D(const QClearBuffers);
    return d->m_clearStencilValue;
}

QRenderTargetOutput *QClearBuffers::colorBuffer() const
{
    Q_D(const QClearBuffers);
    return d->m_buffer;
}

/*!
    \property Qt3DRender::QClearBuffers::buffers
    Specifies the buffer type to be used.
 */

/*!
    \qmlproperty enumeration Qt3D.Render::ClearBuffers::buffers
    Specifies the buffer type to be used.
*/
void QClearBuffers::setBuffers(QClearBuffers::BufferType buffers)
{
    Q_D(QClearBuffers);
    if (d->m_buffersType != buffers) {
        d->m_buffersType = buffers;
        emit buffersChanged(buffers);
    }
}

/*!
    \property Qt3DRender::QClearBuffers::clearColor
    Specifies the clear color to be used.
 */
/*!
    \qmlproperty color Qt3D.Render::ClearBuffers::clearColor
    Specifies the clear color to be used.
*/
void QClearBuffers::setClearColor(const QColor &color)
{
    Q_D(QClearBuffers);
    if (d->m_clearColor != color) {
        d->m_clearColor = color;
        emit clearColorChanged(color);
    }
}


/*!
    \property Qt3DRender::QClearBuffers::clearDepthValue
    Specifies the clear depth value to be used.
 */
/*!
    \qmlproperty real Qt3D.Render::ClearBuffers::clearDepthValue
    Specifies the clear depth value to be used.
*/
void QClearBuffers::setClearDepthValue(float clearDepthValue)
{
    Q_D(QClearBuffers);
    if (d->m_clearDepthValue != clearDepthValue) {
        if (clearDepthValue >= 0.f && clearDepthValue <= 1.f) {
            d->m_clearDepthValue = clearDepthValue;
            emit clearDepthValueChanged(clearDepthValue);
        } else qWarning() << "Invalid clear depth value";
    }
}


/*!
    \property Qt3DRender::QClearBuffers::clearStencilValue
    Specifies the stencil value to be used.
 */
/*!
    \qmlproperty int Qt3D.Render::ClearBuffers::clearStencilValue
    Specifies the stencil value to be used.
*/
void QClearBuffers::setClearStencilValue(int clearStencilValue)
{
    Q_D(QClearBuffers);
    if (d->m_clearStencilValue != clearStencilValue) {
        d->m_clearStencilValue = clearStencilValue;
        emit clearStencilValueChanged(clearStencilValue);
    }
}

/*!
    \property Qt3DRender::QClearBuffers::colorBuffer
    Specifies a specific color buffer to clear. If set to NULL (default), and
    ColorBuffer flag is set, all color buffers will be cleared.
 */
/*!
    \qmlproperty RenderTargetOutput Qt3D.Render::ClearBuffers::colorBuffer
    Specifies a specific color buffer to clear. If set to NULL (default), and
    ColorBuffer flag is set, all color buffers will be cleared.
*/
void QClearBuffers::setColorBuffer(QRenderTargetOutput *buffer)
{
    Q_D(QClearBuffers);
    if (d->m_buffer != buffer) {
        d->m_buffer = buffer;
        emit colorBufferChanged(buffer);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qclearbuffers.cpp"
