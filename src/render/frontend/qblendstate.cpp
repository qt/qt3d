/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qblendstate.h"
#include "qrenderstate_p.h"
#include <Qt3DCore/qscenepropertychange.h>
#include <private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QBlendStatePrivate : public QRenderStatePrivate
{
public:
    QBlendStatePrivate(QRenderState::Type type = QRenderState::BlendState)
        : QRenderStatePrivate(type)
        , m_srcRGB(QBlendState::Zero)
        , m_srcAlpha(QBlendState::Zero)
        , m_dstRGB(QBlendState::Zero)
        , m_dstAlpha(QBlendState::Zero)
    {
    }

    Q_DECLARE_PUBLIC(QBlendState)

    QBlendState::Blending m_srcRGB;
    QBlendState::Blending m_srcAlpha;
    QBlendState::Blending m_dstRGB;
    QBlendState::Blending m_dstAlpha;
};

/*!
    \class Qt3D::QBlendState
    \inmodule Qt3DRenderer
    \since 5.5
    \brief Encapsulates blending information.
 */

/*!
    \qmltype BlendState
    \instantiates Qt3D::QBlendState
    \inherits RenderState
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief Encapsulates blending information
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

/*! \fn void QBlendState::copy(const QNode *ref)
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
}

/*!
  \internal
*/
QBlendState::QBlendState(QRenderState::Type type, QNode *parent)
    : QRenderState(*new QBlendStatePrivate(type), parent)
{
}

/*!
  \enum Qt3D::QBlendState::Blending

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
    \qmlproperty enumeration Qt3D.render::BlendState::srcRGB

 */

/*!
    \property Qt3D::QBlendState::srcRGB

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
        emit srcRGBChanged();
    }
}

/*!
    \qmlproperty enumeration Qt3D.render::BlendState::dstRGB

 */

/*!
    \property Qt3D::QBlendState::dstRGB

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
        emit dstRGBChanged();
    }
}

/*!
    \qmlproperty enumeration Qt3D.render::BlendState::srcAlpha

 */

/*!
    \property Qt3D::QBlendState::srcAlpha

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
        emit srcAlphaChanged();
    }
}

/*!
    \qmlproperty enumeration Qt3D.render::BlendState::dstAlpha

 */

/*!
    \property Qt3D::QBlendState::dstAlpha

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
        emit dstAlphaChanged();
    }
}

/*!
    \class QBlendStateSeparate
    \inmodule Qt3DRenderer
    \since 5.5
    \brief Encapsulates blending information.
 */

/*!
    \qmltype BlendStateSeparate
    \instantiates Qt3D::QBlendStateSeparate
    \inherits RenderState
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief Encapsulates blending information
*/

/*!
  The constructor creates a QBlendStateSeparate with the
  specified \a parent.
 */
QBlendStateSeparate::QBlendStateSeparate(QNode *parent)
    : QBlendState(QRenderState::BlendStateSeparate, parent)
{
}

} // Qt3D

QT_END_NAMESPACE
