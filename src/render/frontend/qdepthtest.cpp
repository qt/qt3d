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

#include "qdepthtest.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QDepthTest;

class QDepthTestPrivate : public QRenderStatePrivate
{
public :
    QDepthTestPrivate()
        : QRenderStatePrivate(QRenderState::DepthTest)
        , m_func(QDepthTest::Never)
    {
    }

    Q_DECLARE_PUBLIC(QDepthTest)
    QDepthTest::DepthFunc m_func;
};

void QDepthTest::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QDepthTest *refState = static_cast<const QDepthTest*>(ref);
    d_func()->m_func = refState->d_func()->m_func;
}

QDepthTest::QDepthTest(QNode *parent)
    : QRenderState(*new QDepthTestPrivate, parent)
{
}

QDepthTest::~QDepthTest()
{
    QNode::cleanup();
}

QDepthTest::DepthFunc QDepthTest::func() const
{
    Q_D(const QDepthTest);
    return d->m_func;
}

void QDepthTest::setFunc(QDepthTest::DepthFunc func)
{
    Q_D(QDepthTest);
    if (d->m_func != func) {
        d->m_func = func;
        emit funcChanged();
    }
}

} // Qt3D

QT_END_NAMESPACE
