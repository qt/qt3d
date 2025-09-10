// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qstenciloperation.h"
#include "qstenciloperation_p.h"
#include "qstenciloperationarguments.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QStencilOperationPrivate::~QStencilOperationPrivate() = default;

/*!
    \class Qt3DRender::QStencilOperation
    \brief The QStencilOperation class specifies stencil operation.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    A Qt3DRender::QStencilOperation class specifies the stencil operations
    for the front- and back-facing polygons. The stencil operation control
    what is done to fragment when the stencil and depth test pass or fail.

    \sa Qt3DRender::QStencilTest
 */

/*!
    \qmltype StencilOperation
    \brief The StencilOperation type specifies stencil operation.
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \inherits RenderState
    \nativetype Qt3DRender::QStencilOperation

    A StencilOperation type specifies the stencil operations
    for the front- and back-facing polygons. The stencil operation control
    what is done to fragment when the stencil and depth test pass or fail.

    \sa StencilTest
 */

/*!
    \qmlproperty StencilOperationArguments StencilOperation::front
    Holds the stencil operation arguments for front-facing polygons.
*/

/*!
    \qmlproperty StencilOperationArguments StencilOperation::back
    Holds the stencil operation arguments for back-facing polygons.
*/

/*!
    \property Qt3DRender::QStencilOperation::front
    Holds the stencil operation arguments for front-facing polygons.
*/

/*!
    \property Qt3DRender::QStencilOperation::back
    Holds the stencil operation arguments for back-facing polygons.
*/

/*!
    The constructor creates a new QStencilOperation::QStencilOperation instance with
    the specified \a parent.
 */
QStencilOperation::QStencilOperation(QNode *parent)
    : QRenderState(*new QStencilOperationPrivate(), parent)
{
    Q_D(QStencilOperation);

    const auto resend = [d]() { d->update(); };

    (void) connect(d->m_front, &QStencilOperationArguments::allTestsPassOperationChanged, this, resend);
    (void) connect(d->m_front, &QStencilOperationArguments::depthTestFailureOperationChanged, this, resend);
    (void) connect(d->m_front, &QStencilOperationArguments::stencilTestFailureOperationChanged, this, resend);
    (void) connect(d->m_front, &QStencilOperationArguments::faceModeChanged, this, resend);

    (void) connect(d->m_back, &QStencilOperationArguments::allTestsPassOperationChanged, this, resend);
    (void) connect(d->m_back, &QStencilOperationArguments::depthTestFailureOperationChanged, this, resend);
    (void) connect(d->m_back, &QStencilOperationArguments::stencilTestFailureOperationChanged, this, resend);
    (void) connect(d->m_back, &QStencilOperationArguments::faceModeChanged, this, resend);
}

/*! \internal */
QStencilOperation::~QStencilOperation()
{
}

/*! \internal */
void QStencilOperationPrivate::fillData(QStencilOperationData &data) const
{
    data.front.face = m_front->faceMode();
    data.front.stencilTestFailureOperation = m_front->stencilTestFailureOperation();
    data.front.depthTestFailureOperation = m_front->depthTestFailureOperation();
    data.front.allTestsPassOperation = m_front->allTestsPassOperation();
    data.back.face = m_back->faceMode();
    data.back.stencilTestFailureOperation = m_back->stencilTestFailureOperation();
    data.back.depthTestFailureOperation = m_back->depthTestFailureOperation();
    data.back.allTestsPassOperation = m_back->allTestsPassOperation();
}

QStencilOperationArguments *QStencilOperation::front() const
{
    Q_D(const QStencilOperation);
    return d->m_front;
}

QStencilOperationArguments *QStencilOperation::back() const
{
    Q_D(const QStencilOperation);
    return d->m_back;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qstenciloperation.cpp"
