// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qstenciltest.h"
#include "qstenciltest_p.h"
#include "qstenciltestarguments.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QStencilTest
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender
    \brief The QStencilTest class specifies arguments for the stecil test.

    A Qt3DRender::QStencilTest class specifies argument for the stencil test.
    The stencil test comprises of three arguments: stencil test function,
    stencil test mask and stencil reference value. QStencilTest allows these
    arguments to be set for both front- and back-facing polygons separately.

    \sa Qt3DRender::QStencilMask, Qt3DRender::QStencilOperation
 */

/*!
    \qmltype StencilTest
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \brief The StencilTest type specifies arguments for the stecil test.
    \inherits RenderState
    \instantiates Qt3DRender::QStencilTest

    A StencilTest type specifies argument for the stencil test.
    The stencil test comprises of three arguments: stencil test function,
    stencil test mask and stencil reference value. StencilTest allows these
    arguments to be set for both front- and back-facing polygons separately.

    \sa StencilMask, StencilOperation
 */

/*!
    \qmlproperty StencilTestArguments StencilTest::front
    Holds the stencil test arguments for front-facing polygons.
*/

/*!
    \qmlproperty StencilTestArguments StencilTest::back
    Holds the stencil test arguments for back-facing polygons.
*/

/*!
    \property QStencilTest::front
    Holds the stencil test arguments for front-facing polygons.
*/

/*!
    \property QStencilTest::back
    Holds the stencil test arguments for back-facing polygons.
*/

/*!
    The constructor creates a new QStencilTest::QStencilTest instance with
    the specified \a parent.
 */
QStencilTest::QStencilTest(QNode *parent)
    : QRenderState(*new QStencilTestPrivate, parent)
{
    Q_D(QStencilTest);

    const auto resend = [d]() { d->update(); };

    (void) connect(d->m_front, &QStencilTestArguments::comparisonMaskChanged, resend);
    (void) connect(d->m_front, &QStencilTestArguments::faceModeChanged, resend);
    (void) connect(d->m_front, &QStencilTestArguments::referenceValueChanged, resend);
    (void) connect(d->m_front, &QStencilTestArguments::stencilFunctionChanged, resend);

    (void) connect(d->m_back, &QStencilTestArguments::comparisonMaskChanged, resend);
    (void) connect(d->m_back, &QStencilTestArguments::faceModeChanged, resend);
    (void) connect(d->m_back, &QStencilTestArguments::referenceValueChanged, resend);
    (void) connect(d->m_back, &QStencilTestArguments::stencilFunctionChanged, resend);
}

/*! \internal */
QStencilTest::~QStencilTest()
{
}

/*! \internal */
void QStencilTestPrivate::fillData(QStencilTestData &data) const
{
    data.front.face = m_front->faceMode();
    data.front.comparisonMask = m_front->comparisonMask();
    data.front.referenceValue = m_front->referenceValue();
    data.front.stencilFunction = m_front->stencilFunction();
    data.back.face = m_back->faceMode();
    data.back.comparisonMask = m_back->comparisonMask();
    data.back.referenceValue = m_back->referenceValue();
    data.back.stencilFunction = m_back->stencilFunction();
}

QStencilTestArguments *QStencilTest::front() const
{
    Q_D(const QStencilTest);
    return d->m_front;
}

QStencilTestArguments *QStencilTest::back() const
{
    Q_D(const QStencilTest);
    return d->m_back;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qstenciltest.cpp"
