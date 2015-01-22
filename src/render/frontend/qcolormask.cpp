/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
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
    QColorMaskPrivate(QColorMask *qq)
        : QRenderStatePrivate(qq)
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

    \brief QColorMask allows to specify which color components should be written
    to the currently bound frame buffer.

    By default each color component (red, green, blue, alpha) is set to true
    which means they will be written to the frame buffer. Setting any of the
    color component to false will prevent it from being written into the frame
    buffer.
 */


/*!
    Constructs a new Qt3D::QColorMask instance with \a parent as parent.
 */
QColorMask::QColorMask(QNode *parent)
    : QRenderState(*new QColorMaskPrivate(this), parent)
{
}

QColorMask::~QColorMask()
{
}

/*!
    \return whether the red color component should be written to the frame buffer;
 */
bool QColorMask::isRed() const
{
    Q_D(const QColorMask);
    return d->m_red;
}

/*!
    \return whether the green color component should be written to the frame buffer;
 */
bool QColorMask::isGreen() const
{
    Q_D(const QColorMask);
    return d->m_green;
}

/*!
    \return whether the blue color component should be written to the frame buffer;
 */
bool QColorMask::isBlue() const
{
    Q_D(const QColorMask);
    return d->m_blue;
}

/*!
    \return whether the alpha color component should be written to the frame buffer;
 */
bool QColorMask::isAlpha() const
{
    Q_D(const QColorMask);
    return d->m_alpha;
}

/*!
    Sets whether the \a red color component should be written to the frame buffer;
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
    Sets whether the \a greem color component should be written to the frame buffer;
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
    Sets whether the \a blue color component should be written to the frame buffer;
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
    Sets whether the \a alpha color component should be written to the frame buffer;
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

