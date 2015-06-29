/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qcolormask.h"
#include <Qt3DRenderer/private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QColorMaskPrivate : public QRenderStatePrivate
{
public:
    QColorMaskPrivate()
        : QRenderStatePrivate(QRenderState::ColorMask)
        , m_red(true)
        , m_green(true)
        , m_blue(true)
        , m_alpha(true)
    {}

    bool m_red;
    bool m_green;
    bool m_blue;
    bool m_alpha;

    Q_DECLARE_PUBLIC(QColorMask)
};

/*!
    \class Qt3D::QColorMask

    \brief Allows specifying which color components should be written to the
    currently bound frame buffer.

    By default, the property for each color component (red, green, blue, alpha)
    is set to \c true which means they will be written to the frame buffer.
    Setting any of the color component to \c false will prevent it from being
    written into the frame buffer.
 */


/*!
    Constructs a new Qt3D::QColorMask instance with \a parent as parent.
 */
QColorMask::QColorMask(QNode *parent)
    : QRenderState(*new QColorMaskPrivate, parent)
{
}

QColorMask::~QColorMask()
{
    QNode::cleanup();
}

bool QColorMask::isRed() const
{
    Q_D(const QColorMask);
    return d->m_red;
}

bool QColorMask::isGreen() const
{
    Q_D(const QColorMask);
    return d->m_green;
}

bool QColorMask::isBlue() const
{
    Q_D(const QColorMask);
    return d->m_blue;
}

bool QColorMask::isAlpha() const
{
    Q_D(const QColorMask);
    return d->m_alpha;
}

/*!
    \property Qt3D::QColorMask::red
    Holds whether the red color component should be written to the frame buffer.
 */
void QColorMask::setRed(bool red)
{
    Q_D(QColorMask);
    if (red != d->m_red) {
        d->m_red = red;
        emit redChanged();
    }
}

/*!
    \property Qt3D::QColorMask::green
    Holds whether the green color component should be written to the frame buffer.
 */
void QColorMask::setGreen(bool green)
{
    Q_D(QColorMask);
    if (green != d->m_green) {
        d->m_green = green;
        emit greenChanged();
    }
}

/*!
    \property Qt3D::QColorMask::blue
    Holds whether the blue color component should be written to the frame buffer.
 */
void QColorMask::setBlue(bool blue)
{
    Q_D(QColorMask);
    if (blue != d->m_blue) {
        d->m_blue = blue;
        emit blueChanged();
    }
}

/*!
    \property Qt3D::QColorMask::alpha
    Holds whether the alpha component should be written to the frame buffer.
 */
void QColorMask::setAlpha(bool alpha)
{
    Q_D(QColorMask);
    if (alpha != d->m_alpha) {
        d->m_alpha = alpha;
        emit alphaChanged();
    }
}

void QColorMask::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QColorMask *refState = static_cast<const QColorMask *>(ref);
    d_func()->m_red = refState->d_func()->m_red;
    d_func()->m_green = refState->d_func()->m_green;
    d_func()->m_blue = refState->d_func()->m_blue;
    d_func()->m_alpha = refState->d_func()->m_alpha;
}

} // Qt3D

QT_END_NAMESPACE

