/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "qblendstate.h"
#include "qrenderstate_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBlendStatePrivate : public QRenderStatePrivate
{
public:
    QBlendStatePrivate(QRenderState::Type type = QRenderState::BlendState)
        : QRenderStatePrivate(type)
        , m_srcRGB(QBlendState::Zero)
        , m_srcAlpha(QBlendState::Zero)
        , m_dstRGB(QBlendState::Zero)
        , m_dstAlpha(QBlendState::Zero)
        , m_enabled(true)
        , m_bufferIndex(-1)
    {
    }

    Q_DECLARE_PUBLIC(QBlendState)

    QBlendState::Blending m_srcRGB;
    QBlendState::Blending m_srcAlpha;
    QBlendState::Blending m_dstRGB;
    QBlendState::Blending m_dstAlpha;
    bool                  m_enabled;
    int                   m_bufferIndex;
};

/*!
    \class Qt3DRender::QBlendState
    \inmodule Qt3DRender
    \since 5.5
    \brief Encapsulates blending information.

    OpenGL pre-3.0:     Set the same blend state for all draw buffers
                        (one QBlendState)
    OpenGL 3.0-pre4.0:  Set the same blend state for all draw buffers,
                        but can disable blending for particular buffers
                        (one QBlendState for setting glBlendFunc, n QBlendStates
                         for enabling/disabling Draw Buffers)
    OpenGL 4.0+:        Can set blend state individually for each draw buffer.
 */

/*!
    \qmltype BlendState
    \instantiates Qt3DRender::QBlendState
    \inherits RenderState
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief Encapsulates blending information.

    OpenGL pre-3.0:     Set the same blend state for all draw buffers
    OpenGL 3.0-pre4.0:  Set the same blend state for all draw buffers,
                        but can disable blending for particular buffers
    OpenGL 4.0+:        Can set blend state individually for each draw buffer.
*/

/*!
  The constructor creates a new blend state object with the specified \a parent.
 */
QBlendState::QBlendState(QNode *parent)
    : QRenderState(*new QBlendStatePrivate, parent)
{
}

QBlendState::~QBlendState()
{
    QNode::cleanup();
}

/*! \fn void QBlendState::copy(const Qt3DCore::QNode *ref)
  \internal
  Copies \a ref into this object.
 */
void QBlendState::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QBlendState *refState = static_cast<const QBlendState*>(ref);
    d_func()->m_srcRGB = refState->d_func()->m_srcRGB;
    d_func()->m_srcAlpha = refState->d_func()->m_srcAlpha;
    d_func()->m_dstAlpha = refState->d_func()->m_dstAlpha;
    d_func()->m_dstRGB = refState->d_func()->m_dstRGB;
    d_func()->m_enabled = refState->d_func()->m_enabled;
    d_func()->m_bufferIndex = refState->d_func()->m_bufferIndex;
}

/*!
  \internal
*/
QBlendState::QBlendState(QRenderState::Type type, QNode *parent)
    : QRenderState(*new QBlendStatePrivate(type), parent)
{
}

/*!
  \enum Qt3DRender::QBlendState::Blending

  \value Zero 0
  \value One 1
  \value SrcColor 0x0300
  \value SrcAlpha 0x0302
  \value Src1Alpha
  \value Src1Color
  \value DstColor 0x0306
  \value DstAlpha 0x0304
  \value SrcAlphaSaturate 0x0308
  \value ConstantColor 0x8001
  \value ConstantAlpha 0x8003
  \value OneMinusSrcColor 0x0301
  \value OneMinusSrcAlpha 0x0303
  \value OneMinusDstAlpha 0x0305
  \value OneMinusDstColor 0x0307
  \value OneMinusConstantColor 0x8002
  \value OneMinusConstantAlpha 0x8004
  \value OneMinusSrc1Alpha
  \value OneMinusSrc1Color0
*/

/*!
    \qmlproperty enumeration Qt3D.Render::BlendState::srcRGB

 */

/*!
    \property Qt3DRender::QBlendState::srcRGB

 */
QBlendState::Blending QBlendState::srcRGB() const
{
    Q_D(const QBlendState);
    return d->m_srcRGB;
}

void QBlendState::setSrcRGB(QBlendState::Blending srcRGB)
{
    Q_D(QBlendState);
    if (d->m_srcRGB != srcRGB) {
        d->m_srcRGB = srcRGB;
        emit srcRGBChanged(srcRGB);
    }
}

/*!
    \qmlproperty enumeration Qt3D.Render::BlendState::dstRGB

 */

/*!
    \property Qt3DRender::QBlendState::dstRGB

 */
QBlendState::Blending QBlendState::dstRGB() const
{
    Q_D(const QBlendState);
    return d->m_dstRGB;
}

void QBlendState::setDstRGB(QBlendState::Blending dstRGB)
{
    Q_D(QBlendState);
    if (d->m_dstRGB != dstRGB) {
        d->m_dstRGB = dstRGB;
        emit dstRGBChanged(dstRGB);
    }
}

/*!
    \qmlproperty enumeration Qt3D.Render::BlendState::srcAlpha

 */

/*!
    \property Qt3DRender::QBlendState::srcAlpha

 */
QBlendState::Blending QBlendState::srcAlpha() const
{
    Q_D(const QBlendState);
    return d->m_srcAlpha;
}

void QBlendState::setSrcAlpha(QBlendState::Blending srcAlpha)
{
    Q_D(QBlendState);
    if (d->m_srcAlpha != srcAlpha) {
        d->m_srcAlpha = srcAlpha;
        emit srcAlphaChanged(srcAlpha);
    }
}

/*!
    \qmlproperty enumeration Qt3D.Render::BlendState::dstAlpha

 */

/*!
    \property Qt3DRender::QBlendState::dstAlpha

 */
QBlendState::Blending QBlendState::dstAlpha() const
{
    Q_D(const QBlendState);
    return d->m_dstAlpha;
}

void QBlendState::setDstAlpha(QBlendState::Blending dstAlpha)
{
    Q_D(QBlendState);
    if (d->m_dstAlpha != dstAlpha) {
        d->m_dstAlpha = dstAlpha;
        emit dstAlphaChanged(dstAlpha);
    }
}

bool QBlendState::enabled() const
{
    Q_D(const QBlendState);
    return d->m_enabled;
}

void QBlendState::setEnabled(bool enabled)
{
    Q_D(QBlendState);
    if (d->m_enabled != enabled) {
        d->m_enabled = enabled;
        emit enabledChanged(enabled);
    }
}

/*!
    \qmlproperty int Qt3D.Render::BlendState::bufferIndex

    Specifies the index of the Draw Buffer that this BlendState applies to.
    If negative, this will apply to all Draw Buffers.
 */

/*!
    \property Qt3DRender::QBlendState::bufferIndex

    Specifies the index of the Draw Buffer that this BlendState applies to.
    If negative, this will apply to all Draw Buffers.
 */
int QBlendState::bufferIndex() const
{
    Q_D(const QBlendState);
    return d->m_bufferIndex;
}

void QBlendState::setBufferIndex(int bufferIndex)
{
    Q_D(QBlendState);
    if (d->m_bufferIndex != bufferIndex) {
        bool oldAllBuffers = (d->m_bufferIndex < 0);
        bool newAllBuffers = (bufferIndex < 0);

        d->m_bufferIndex = bufferIndex;
        emit bufferIndexChanged(bufferIndex);

        if (oldAllBuffers != newAllBuffers)
            emit specifiesAllDrawBuffersChanged(newAllBuffers);
    }
}

bool QBlendState::specifiesAllDrawBuffers() const
{
    Q_D(const QBlendState);
    return (d->m_bufferIndex < 0);
}

/*!
    \class Qt3DRender::QBlendStateSeparate
    \inmodule Qt3DRender
    \since 5.5
    \brief Encapsulates blending information.
 */

/*!
    \qmltype BlendStateSeparate
    \instantiates Qt3DRender::QBlendStateSeparate
    \inherits RenderState
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief Encapsulates blending information.
*/

/*!
  The constructor creates a QBlendStateSeparate instance with the
  specified \a parent.
 */
QBlendStateSeparate::QBlendStateSeparate(QNode *parent)
    : QBlendState(QRenderState::BlendStateSeparate, parent)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE
