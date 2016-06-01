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

/*!
 * \class QDepthTest
 * \brief The QDepthTest class tests the fragment shader's depth value against
 * the depth of a sample being written to.
 * \since 5.7
 * \ingroup renderstates
 *
 */
#include "qdepthtest.h"
#include "qdepthtest_p.h"
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * The constructor creates a new QDepthTest::QDepthTest instance with the specified \a parent.
 */
QDepthTest::QDepthTest(QNode *parent)
    : QRenderState(*new QDepthTestPrivate, parent)
{
}

/*! \internal */
QDepthTest::~QDepthTest()
{
}

/*!
 * \return the current enabled depth function.
 */
QDepthTest::DepthFunction QDepthTest::depthFunction() const
{
    Q_D(const QDepthTest);
    return d->m_depthFunction;
}

/*!
 * Sets the depth function being enabled to \a depthFunction
 * \param depthFunction
 */
void QDepthTest::setDepthFunction(QDepthTest::DepthFunction depthFunction)
{
    Q_D(QDepthTest);
    if (d->m_depthFunction != depthFunction) {
        d->m_depthFunction = depthFunction;
        emit depthFunctionChanged(depthFunction);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QDepthTest::createNodeCreationChange() const
{
    auto creationChange = QRenderStateCreatedChangePtr<QDepthTestData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QDepthTest);
    data.depthFunction = d->m_depthFunction;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
