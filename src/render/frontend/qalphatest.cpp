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

#include "qalphatest.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAlphaTestPrivate : public QRenderStatePrivate
{
public:
    QAlphaTestPrivate()
        : QRenderStatePrivate(QRenderState::AlphaTest)
        , m_func(QAlphaTest::Never)
        , m_clamp(0.0f)
    {
    }

    Q_DECLARE_PUBLIC(QAlphaTest)
    QAlphaTest::AlphaFunc m_func;
    float m_clamp;
};

QAlphaTest::QAlphaTest(QNode *parent)
    : QRenderState(*new QAlphaTestPrivate, parent)
{
}

QAlphaTest::~QAlphaTest()
{
    QNode::cleanup();
}

void QAlphaTest::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QAlphaTest *refState = static_cast<const QAlphaTest*>(ref);
    d_func()->m_func = refState->d_func()->m_func;
    d_func()->m_clamp = refState->d_func()->m_clamp;
}

QAlphaTest::AlphaFunc QAlphaTest::func() const
{
    Q_D(const QAlphaTest);
    return d->m_func;
}

void QAlphaTest::setFunc(QAlphaTest::AlphaFunc func)
{
    Q_D(QAlphaTest);
    if (d->m_func != func) {
        d->m_func = func;
        emit funcChanged();
    }
}

float QAlphaTest::clamp() const
{
    Q_D(const QAlphaTest);
    return d->m_clamp;
}

void QAlphaTest::setClamp(float clamp)
{
    Q_D(QAlphaTest);
    if (d->m_clamp != clamp) {
        d->m_clamp = clamp;
        emit clampChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
