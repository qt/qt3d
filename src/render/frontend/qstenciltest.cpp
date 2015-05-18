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

#include "qstenciltest.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>


QT_BEGIN_NAMESPACE

namespace Qt3D {

class QStencilTestPrivate : public QRenderStatePrivate
{
public:
    QStencilTestPrivate()
        : QRenderStatePrivate(QRenderState::StencilTest)
        , m_mask(0)
        , m_func(QStencilTest::Never)
        , m_faceMode(QStencilTest::FrontAndBack)
    {
    }

    Q_DECLARE_PUBLIC(QStencilTest)
    uint m_mask;
    QStencilTest::StencilFunc m_func;
    QStencilTest::StencilFaceMode m_faceMode;
};

QStencilTest::QStencilTest(QNode *parent)
    : QRenderState(*new QStencilTestPrivate, parent)
{
}

void QStencilTest::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QStencilTest *refState = static_cast<const QStencilTest*>(ref);
    d_func()->m_mask = refState->d_func()->m_mask;
    d_func()->m_faceMode = refState->d_func()->m_faceMode;
    d_func()->m_func = refState->d_func()->m_func;
}

uint QStencilTest::mask() const
{
    Q_D(const QStencilTest);
    return d->m_mask;
}

void QStencilTest::setMask(uint mask)
{
    Q_D(QStencilTest);
    if (d->m_mask != mask) {
        d->m_mask = mask;
        emit maskChanged();
    }
}

QStencilTest::StencilFaceMode QStencilTest::faceMode() const
{
    Q_D(const QStencilTest);
    return d->m_faceMode;
}

void QStencilTest::setFaceMode(QStencilTest::StencilFaceMode mode)
{
    Q_D(QStencilTest);
    if (d->m_faceMode != mode) {
        d->m_faceMode = mode;
        emit faceModeChanged();
    }
}

QStencilTest::StencilFunc QStencilTest::func() const
{
    Q_D(const QStencilTest);
    return d->m_func;
}

void QStencilTest::setFunc(QStencilTest::StencilFunc func)
{
    Q_D(QStencilTest);
    if (d->m_func != func) {
        d->m_func = func;
        emit funcChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
