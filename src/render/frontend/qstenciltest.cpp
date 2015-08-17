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
#include "qstenciltestseparate.h"


QT_BEGIN_NAMESPACE

namespace Qt3D {

class QStencilTestPrivate : public QRenderStatePrivate
{
public:
    QStencilTestPrivate()
        : QRenderStatePrivate(QRenderState::StencilTest)
        , m_front(new QStencilTestSeparate(QStencilTestSeparate::Front))
        , m_back(new QStencilTestSeparate(QStencilTestSeparate::Back))
    {
    }

    Q_DECLARE_PUBLIC(QStencilTest)
    QStencilTestSeparate *m_front;
    QStencilTestSeparate *m_back;
};

QStencilTest::QStencilTest(QNode *parent)
    : QRenderState(*new QStencilTestPrivate, parent)
{
}

QStencilTest::~QStencilTest()
{
    QNode::cleanup();
}

QStencilTestSeparate *QStencilTest::front() const
{
    Q_D(const QStencilTest);
    return d->m_front;
}

QStencilTestSeparate *QStencilTest::back() const
{
    Q_D(const QStencilTest);
    return d->m_back;
}

void QStencilTest::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QStencilTest *refState = static_cast<const QStencilTest*>(ref);
    d_func()->m_front->setMask(refState->d_func()->m_front->mask());
    d_func()->m_front->setRef(refState->d_func()->m_front->ref());
    d_func()->m_front->setFunc(refState->d_func()->m_front->func());
    d_func()->m_back->setMask(refState->d_func()->m_back->mask());
    d_func()->m_back->setRef(refState->d_func()->m_back->ref());
    d_func()->m_back->setFunc(refState->d_func()->m_back->func());
}


} // Qt3D

QT_END_NAMESPACE
