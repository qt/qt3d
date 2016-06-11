/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "qstenciltest.h"
#include "qstenciltest_p.h"
#include "qstenciltestarguments.h"
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QStencilTest
 * \brief The QStencilTest class
 * \since 5.7
 * \ingroup renderstates
 */

/*!
 * The constructor creates a new QStencilTest::QStencilTest instance with
 * the specified \a parent.
 */
QStencilTest::QStencilTest(QNode *parent)
    : QRenderState(*new QStencilTestPrivate, parent)
{
}

/*! \internal */
QStencilTest::~QStencilTest()
{
}

/*!
 * \return the stencil test for the front
 */
QStencilTestArguments *QStencilTest::front() const
{
    Q_D(const QStencilTest);
    return d->m_front;
}

/*!
 * \return the stencil test for the back
 */
QStencilTestArguments *QStencilTest::back() const
{
    Q_D(const QStencilTest);
    return d->m_back;
}

Qt3DCore::QNodeCreatedChangeBasePtr QStencilTest::createNodeCreationChange() const
{
    auto creationChange = QRenderStateCreatedChangePtr<QStencilTestData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QStencilTest);
    data.front.face = d->m_front->faceMode();
    data.front.comparisonMask = d->m_front->comparisonMask();
    data.front.referenceValue = d->m_front->referenceValue();
    data.front.stencilFunction = d->m_front->stencilFunction();
    data.back.face = d->m_back->faceMode();
    data.back.comparisonMask = d->m_back->comparisonMask();
    data.back.referenceValue = d->m_back->referenceValue();
    data.back.stencilFunction = d->m_back->stencilFunction();
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
