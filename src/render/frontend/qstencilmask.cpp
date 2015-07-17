/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include "qstencilmask.h"
#include <Qt3DRenderer/private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QStencilMaskPrivate : public QRenderStatePrivate
{
public:
    QStencilMaskPrivate()
        : QRenderStatePrivate(QRenderState::StencilMask)
        , m_frontMask(0)
        , m_backMask(0)
    {}

    uint m_frontMask;
    uint m_backMask;
};

/*!
 * QStencilMask::QStencilMask
 */

QStencilMask::QStencilMask(QNode *parent)
    : QRenderState(*new QStencilMaskPrivate(), parent)
{
}

QStencilMask::~QStencilMask()
{
    QNode::cleanup();
}

void QStencilMask::setFrontMask(uint mask)
{
    Q_D(QStencilMask);
    if (d->m_frontMask != mask) {
        d->m_frontMask = mask;
        Q_EMIT frontMaskChanged();
    }
}

void QStencilMask::setBackMask(uint mask)
{
    Q_D(QStencilMask);
    if (d->m_backMask != mask) {
        d->m_backMask = mask;
        Q_EMIT backMaskChanged();
    }
}

uint QStencilMask::frontMask() const
{
    Q_D(const QStencilMask);
    return d->m_frontMask;
}

uint QStencilMask::backMask() const
{
    Q_D(const QStencilMask);
    return d->m_backMask;
}

void QStencilMask::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QStencilMask *otherRef = static_cast<const QStencilMask *>(ref);
    d_func()->m_frontMask = otherRef->frontMask();
    d_func()->m_backMask = otherRef->backMask();
}

} // Qt3D

QT_END_NAMESPACE
