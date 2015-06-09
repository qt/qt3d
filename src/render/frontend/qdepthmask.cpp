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

#include "qdepthmask.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QDepthMaskPrivate : public QRenderStatePrivate
{
public:
    QDepthMaskPrivate()
        : QRenderStatePrivate(QRenderState::DepthMask)
        , m_mask(false)
    {
    }

    Q_DECLARE_PUBLIC(QDepthMask)
    bool m_mask;
};

QDepthMask::QDepthMask(QNode *parent)
    : QRenderState(*new QDepthMaskPrivate, parent)
{
}

QDepthMask::~QDepthMask()
{
    QNode::cleanup();
}

void QDepthMask::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QDepthMask *refState = static_cast<const QDepthMask*>(ref);
    d_func()->m_mask = refState->d_func()->m_mask;
}

bool QDepthMask::mask() const
{
    Q_D(const QDepthMask);
    return d->m_mask;
}

void QDepthMask::setMask(bool mask)
{
    Q_D(QDepthMask);
    if (d->m_mask != mask) {
        d->m_mask = mask;
        emit maskChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
