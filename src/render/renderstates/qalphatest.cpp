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
 * \class QAlphaTest
 * \brief The QAlphaTest class is an OpenGL helper.
 * \since 5.7
 * \ingroup renderstates
 *
 * As the OpenGL documentation explains; The alpha test discards a fragment
 * conditional on the outcome of a comparison between the incoming fragment's
 * alpha value and a constant value.
 */

#include "qalphatest.h"
#include "qalphatest_p.h"
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QAlphaTest::QAlphaTest(QNode *parent)
    : QRenderState(*new QAlphaTestPrivate, parent)
{
}

/*! \internal */
QAlphaTest::~QAlphaTest()
{
}

/*!
 * \return the current alpha test function.
 */
QAlphaTest::AlphaFunction QAlphaTest::alphaFunction() const
{
    Q_D(const QAlphaTest);
    return d->m_alphaFunction;
}

/*!
 * Sets the alpha test function to \a alphaFunction.
 * \param alphaFunction
 */
void QAlphaTest::setAlphaFunction(QAlphaTest::AlphaFunction alphaFunction)
{
    Q_D(QAlphaTest);
    if (d->m_alphaFunction != alphaFunction) {
        d->m_alphaFunction = alphaFunction;
        emit alphaFunctionChanged(alphaFunction);
    }
}

/*!
 * \return a float value between 0 and 1.
 */
float QAlphaTest::referenceValue() const
{
    Q_D(const QAlphaTest);
    return d->m_referenceValue;
}

/*!
 * Sets the reference value which is clamped between 0 and 1 to \a referenceValue.
 */
void QAlphaTest::setReferenceValue(float referenceValue)
{
    Q_D(QAlphaTest);
    if (d->m_referenceValue != referenceValue) {
        d->m_referenceValue = referenceValue;
        emit referenceValueChanged(referenceValue);
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr QAlphaTest::createNodeCreationChange() const
{
    auto creationChange = QRenderStateCreatedChangePtr<QAlphaTestData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QAlphaTest);
    data.alphaFunction = d->m_alphaFunction;
    data.referenceValue = d->m_referenceValue;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
