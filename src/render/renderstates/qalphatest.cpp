// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qalphatest.h"
#include "qalphatest_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QAlphaTest
    \brief The QAlphaTest class specify alpha reference test.
    \since 5.7
    \inmodule Qt3DRender
    \ingroup renderstates

    As the OpenGL documentation explains; The alpha test discards a fragment
    conditional on the outcome of a comparison between the incoming fragment's
    alpha value and a constant reference value.
 */

/*!
    \qmltype AlphaTest
    \brief The AlphaTest class specify alpha reference test.
    \since 5.7
    \inqmlmodule Qt3D.Render
    \inherits RenderState
    \nativetype Qt3DRender::QAlphaTest
    \ingroup renderstates

    As the OpenGL documentation explains; The alpha test discards a fragment
    conditional on the outcome of a comparison between the incoming fragment's
    alpha value and a constant reference value.
 */

/*!
    \enum Qt3DRender::QAlphaTest::AlphaFunction

    Enumeration for the alpha function values
    \value Never Never pass alpha test
    \value Always Always pass alpha test
    \value Less Pass alpha test if fragment alpha is less than reference value
    \value LessOrEqual Pass alpha test if fragment alpha is less than or equal to reference value
    \value Equal Pass alpha test if fragment alpha is equal to reference value
    \value GreaterOrEqual Pass alpha test if fragment alpha is greater than or equal to reference value
    \value Greater Pass alpha test if fragment alpha is greater than reference value
    \value NotEqual Pass alpha test if fragment alpha is not equal to reference value
*/

/*!
    \qmlproperty enumeration AlphaTest::alphaFunction
    Holds the alpha function used by the alpha test. Default is AlphaTest.Never.
    \list
    \li AlphaTest.Never
    \li AlphaTest.Always
    \li AlphaTest.Less
    \li AlphaTest.LessOrEqual
    \li AlphaTest.Equal
    \li AlphaTest.GreaterOrEqual
    \li AlphaTest.Greater
    \li AlphaTest.NotEqual
    \endlist
    \sa Qt3DRender::QAlphaTest::AlphaFunction
*/

/*!
    \qmlproperty real AlphaTest::referenceValue
    Holds the reference value used by the alpha test. Default is 0.0.
    When set, the value is clamped between 0 and 1.
*/

/*!
    \property Qt3DRender::QAlphaTest::alphaFunction
    Holds the alpha function used by the alpha test. Default is Never.
*/

/*!
    \property Qt3DRender::QAlphaTest::referenceValue
    Holds the reference value used by the alpha test. Default is 0.0.
    When set, the value is clamped between 0 and 1.
*/


QAlphaTest::QAlphaTest(QNode *parent)
    : QRenderState(*new QAlphaTestPrivate, parent)
{
}

/*! \internal */
QAlphaTest::~QAlphaTest()
{
}

QAlphaTest::AlphaFunction QAlphaTest::alphaFunction() const
{
    Q_D(const QAlphaTest);
    return d->m_alphaFunction;
}

void QAlphaTest::setAlphaFunction(QAlphaTest::AlphaFunction alphaFunction)
{
    Q_D(QAlphaTest);
    if (d->m_alphaFunction != alphaFunction) {
        d->m_alphaFunction = alphaFunction;
        emit alphaFunctionChanged(alphaFunction);
    }
}

float QAlphaTest::referenceValue() const
{
    Q_D(const QAlphaTest);
    return d->m_referenceValue;
}

void QAlphaTest::setReferenceValue(float referenceValue)
{
    Q_D(QAlphaTest);
    if (d->m_referenceValue != referenceValue) {
        d->m_referenceValue = referenceValue;
        emit referenceValueChanged(referenceValue);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qalphatest.cpp"
