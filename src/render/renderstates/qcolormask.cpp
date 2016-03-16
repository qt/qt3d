/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qcolormask.h"
#include <Qt3DRender/private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QColorMaskPrivate : public QRenderStatePrivate
{
public:
    QColorMaskPrivate()
        : QRenderStatePrivate(QRenderStatePrivate::ColorMask)
        , m_redMasked(true)
        , m_greenMasked(true)
        , m_blueMasked(true)
        , m_alphaMasked(true)
    {}

    bool m_redMasked;
    bool m_greenMasked;
    bool m_blueMasked;
    bool m_alphaMasked;

    Q_DECLARE_PUBLIC(QColorMask)
};

/*!
    \class Qt3DRender::QColorMask
    \inmodule Qt3DRender

    \brief Allows specifying which color components should be written to the
    currently bound frame buffer.

    By default, the property for each color component (red, green, blue, alpha)
    is set to \c true which means they will be written to the frame buffer.
    Setting any of the color component to \c false will prevent it from being
    written into the frame buffer.
 */


/*!
    Constructs a new Qt3DCore::QColorMask instance with \a parent as parent.
 */
QColorMask::QColorMask(QNode *parent)
    : QRenderState(*new QColorMaskPrivate, parent)
{
}

QColorMask::~QColorMask()
{
    QNode::cleanup();
}

bool QColorMask::isRedMasked() const
{
    Q_D(const QColorMask);
    return d->m_redMasked;
}

bool QColorMask::isGreenMasked() const
{
    Q_D(const QColorMask);
    return d->m_greenMasked;
}

bool QColorMask::isBlueMasked() const
{
    Q_D(const QColorMask);
    return d->m_blueMasked;
}

bool QColorMask::isAlphaMasked() const
{
    Q_D(const QColorMask);
    return d->m_alphaMasked;
}

/*!
    \property Qt3DRender::QColorMask::red
    Holds whether the red color component should be written to the frame buffer.
 */
void QColorMask::setRedMasked(bool redMasked)
{
    Q_D(QColorMask);
    if (redMasked != d->m_redMasked) {
        d->m_redMasked = redMasked;
        emit redMaskedChanged(redMasked);
    }
}

/*!
    \property Qt3DRender::QColorMask::green
    Holds whether the green color component should be written to the frame buffer.
 */
void QColorMask::setGreenMasked(bool greenMasked)
{
    Q_D(QColorMask);
    if (greenMasked != d->m_greenMasked) {
        d->m_greenMasked = greenMasked;
        emit greenMaskedChanged(greenMasked);
    }
}

/*!
    \property Qt3DRender::QColorMask::blue
    Holds whether the blue color component should be written to the frame buffer.
 */
void QColorMask::setBlueMasked(bool blueMasked)
{
    Q_D(QColorMask);
    if (blueMasked != d->m_blueMasked) {
        d->m_blueMasked = blueMasked;
        emit blueMaskedChanged(blueMasked);
    }
}

/*!
    \property Qt3DRender::QColorMask::alphaMasked
    Holds whether the alphaMasked component should be written to the frame buffer.
 */
void QColorMask::setAlphaMasked(bool alphaMasked)
{
    Q_D(QColorMask);
    if (alphaMasked != d->m_alphaMasked) {
        d->m_alphaMasked = alphaMasked;
        emit alphaMaskedChanged(alphaMasked);
    }
}

void QColorMask::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QColorMask *refState = static_cast<const QColorMask *>(ref);
    d_func()->m_redMasked = refState->d_func()->m_redMasked;
    d_func()->m_greenMasked = refState->d_func()->m_greenMasked;
    d_func()->m_blueMasked = refState->d_func()->m_blueMasked;
    d_func()->m_alphaMasked = refState->d_func()->m_alphaMasked;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

