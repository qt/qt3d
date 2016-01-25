/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include "qstencilmask.h"
#include <Qt3DRender/private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

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
        Q_EMIT frontMaskChanged(mask);
    }
}

void QStencilMask::setBackMask(uint mask)
{
    Q_D(QStencilMask);
    if (d->m_backMask != mask) {
        d->m_backMask = mask;
        Q_EMIT backMaskChanged(mask);
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

} // namespace Qt3DRender

QT_END_NAMESPACE
