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

#include "qblendequationarguments.h"
#include "qblendequationarguments_p.h"
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QBlendEquationArguments
    \inmodule Qt3DRender
    \since 5.5
    \brief Encapsulates blending information.

    OpenGL pre-3.0:     Set the same blend state for all draw buffers
                        (one QBlendEquationArguments)
    OpenGL 3.0-pre4.0:  Set the same blend state for all draw buffers,
                        but can disable blending for particular buffers
                        (one QBlendEquationArguments for setting glBlendFunc, n QBlendEquationArgumentss
                         for enabling/disabling Draw Buffers)
    OpenGL 4.0+:        Can set blend state individually for each draw buffer.
 */

/*!
    \qmltype BlendEquationArguments
    \instantiates Qt3DRender::QBlendEquationArguments
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
QBlendEquationArguments::QBlendEquationArguments(QNode *parent)
    : QRenderState(*new QBlendEquationArgumentsPrivate, parent)
{
}

/*!
  \internal
*/
QBlendEquationArguments::~QBlendEquationArguments()
{
}

/*!
  \internal
*/
QBlendEquationArguments::QBlendEquationArguments(QBlendEquationArgumentsPrivate &dd, QNode *parent)
    : QRenderState(dd, parent)
{
}

/*!
  \enum Qt3DRender::QBlendEquationArguments::Blending

  \value Zero 0
  \value One 1
  \value SrcColor 0x0300
  \value SourceAlpha 0x0302
  \value Src1Alpha
  \value Src1Color
  \value DstColor 0x0306
  \value destinationAlpha 0x0304
  \value SourceAlphaSaturate 0x0308
  \value ConstantColor 0x8001
  \value ConstantAlpha 0x8003
  \value OneMinusSrcColor 0x0301
  \value OneMinusSourceAlpha 0x0303
  \value OneMinusdestinationAlpha 0x0305
  \value OneMinusDstColor 0x0307
  \value OneMinusConstantColor 0x8002
  \value OneMinusConstantAlpha 0x8004
  \value OneMinusSrc1Alpha
  \value OneMinusSrc1Color0
*/

/*!
    \qmlproperty enumeration Qt3D.Render::BlendEquationArguments::sourceRgb

 */

/*!
    \property Qt3DRender::QBlendEquationArguments::sourceRgb

 */
QBlendEquationArguments::Blending QBlendEquationArguments::sourceRgb() const
{
    Q_D(const QBlendEquationArguments);
    return d->m_sourceRgb;
}

void QBlendEquationArguments::setSourceRgb(QBlendEquationArguments::Blending sourceRgb)
{
    Q_D(QBlendEquationArguments);
    if (d->m_sourceRgb != sourceRgb) {
        d->m_sourceRgb = sourceRgb;
        emit sourceRgbChanged(sourceRgb);

        if (d->m_sourceAlpha == sourceRgb)
            emit sourceRgbaChanged(sourceRgb);
    }
}

/*!
    \qmlproperty enumeration Qt3D.Render::BlendEquationArguments::destinationRgb

 */

/*!
    \property Qt3DRender::QBlendEquationArguments::destinationRgb

 */
QBlendEquationArguments::Blending QBlendEquationArguments::destinationRgb() const
{
    Q_D(const QBlendEquationArguments);
    return d->m_destinationRgb;
}

void QBlendEquationArguments::setDestinationRgb(QBlendEquationArguments::Blending destinationRgb)
{
    Q_D(QBlendEquationArguments);
    if (d->m_destinationRgb != destinationRgb) {
        d->m_destinationRgb = destinationRgb;
        emit destinationRgbChanged(destinationRgb);

        if (d->m_destinationAlpha == destinationRgb)
            emit destinationRgbaChanged(destinationRgb);
    }
}

/*!
    \qmlproperty enumeration Qt3D.Render::BlendEquationArguments::sourceAlpha

 */

/*!
    \property Qt3DRender::QBlendEquationArguments::sourceAlpha

 */
QBlendEquationArguments::Blending QBlendEquationArguments::sourceAlpha() const
{
    Q_D(const QBlendEquationArguments);
    return d->m_sourceAlpha;
}

void QBlendEquationArguments::setSourceAlpha(QBlendEquationArguments::Blending sourceAlpha)
{
    Q_D(QBlendEquationArguments);
    if (d->m_sourceAlpha != sourceAlpha) {
        d->m_sourceAlpha = sourceAlpha;
        emit sourceAlphaChanged(sourceAlpha);

        if (d->m_sourceRgb == sourceAlpha)
            emit sourceRgbaChanged(sourceAlpha);
    }
}

/*!
    \qmlproperty enumeration Qt3D.Render::BlendEquationArguments::DestinationAlpha

 */

/*!
    \property Qt3DRender::QBlendEquationArguments::destinationAlpha

 */
QBlendEquationArguments::Blending QBlendEquationArguments::destinationAlpha() const
{
    Q_D(const QBlendEquationArguments);
    return d->m_destinationAlpha;
}

void QBlendEquationArguments::setDestinationAlpha(QBlendEquationArguments::Blending destinationAlpha)
{
    Q_D(QBlendEquationArguments);
    if (d->m_destinationAlpha != destinationAlpha) {
        d->m_destinationAlpha = destinationAlpha;
        emit destinationAlphaChanged(destinationAlpha);

        if (d->m_destinationRgb == destinationAlpha)
            emit destinationRgbaChanged(destinationAlpha);
    }
}

void QBlendEquationArguments::setSourceRgba(Blending sourceRgba)
{
    setSourceRgb(sourceRgba);
    setSourceAlpha(sourceRgba);
}

void QBlendEquationArguments::setDestinationRgba(Blending destinationRgba)
{
    setDestinationRgb(destinationRgba);
    setDestinationAlpha(destinationRgba);
}

/*!
    \qmlproperty int Qt3D.Render::BlendEquationArguments::bufferIndex

    Specifies the index of the Draw Buffer that this BlendEquationArguments applies to.
    If negative, this will apply to all Draw Buffers.
 */

/*!
    \property Qt3DRender::QBlendEquationArguments::bufferIndex

    Specifies the index of the Draw Buffer that this BlendEquationArguments applies to.
    If negative, this will apply to all Draw Buffers.
 */
int QBlendEquationArguments::bufferIndex() const
{
    Q_D(const QBlendEquationArguments);
    return d->m_bufferIndex;
}

void QBlendEquationArguments::setBufferIndex(int bufferIndex)
{
    Q_D(QBlendEquationArguments);
    if (d->m_bufferIndex != bufferIndex) {
        d->m_bufferIndex = bufferIndex;
        emit bufferIndexChanged(bufferIndex);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QBlendEquationArguments::createNodeCreationChange() const
{
    auto creationChange = QRenderStateCreatedChangePtr<QBlendEquationArgumentsData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QBlendEquationArguments);
    data.sourceRgb = d->m_sourceRgb;
    data.sourceAlpha = d->m_sourceAlpha;
    data.destinationRgb = d->m_destinationRgb;
    data.destinationAlpha = d->m_destinationAlpha;
    data.bufferIndex = d->m_bufferIndex;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
