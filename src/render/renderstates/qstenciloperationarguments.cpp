// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qstenciloperationarguments.h"
#include "qstenciloperationarguments_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QStencilOperationArgumentsPrivate::~QStencilOperationArgumentsPrivate() = default;

/*!
    \class Qt3DRender::QStencilOperationArguments
    \brief The QStencilOperationArguments class sets the actions to be taken
    when stencil and depth tests fail.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    The Qt3DRender::QStencilOperationArguments class specifies the arguments for
    the stencil operations.

    \sa Qt3DRender::QStencilOperation
 */

/*!
    \qmltype StencilOperationArguments
    \brief The StencilOperationArguments type sets the actions to be taken
    when stencil and depth tests fail.
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QStencilOperationArguments
    \inherits QtObject

    The StencilOperationArguments type specifies the arguments for the stencil operations.

    \sa StencilOperation
 */

/*!
    \enum Qt3DRender::QStencilOperationArguments::FaceMode
    This enumeration holds the values for stencil operation argument face modes
    \value Front Arguments are applied to front-facing polygons.
    \value Back Arguments are applied to back-facing polygons.
    \value FrontAndBack Arguments are applied to both front- and back-facing polygons.
*/

/*!
    \enum Qt3DRender::QStencilOperationArguments::Operation
    This enumeration holds the values for stencil operation.
    \value Zero Set stencil value to zero.
    \value Keep Keep current stencil value.
    \value Replace Replace with the masked fragment stencil value.
    \value Increment Increment current value with saturation.
    \value Decrement Decrement current value with saturation.
    \value IncrementWrap Increment current value with wrap.
    \value DecrementWrap Decrement current value with wrap.
    \value Invert Invert the current value.
*/

/*!
    \qmlproperty enumeration StencilOperationArguments::faceMode
    Holds the faces the arguments are applied to.
    \list
    \li StencilOperationArguments.Front
    \li StencilOperationArguments.Back
    \li StencilOperationArguments.FrontAndBack
    \endlist
    \sa Qt3DRender::QStencilOperationArguments::FaceMode
    \readonly
*/

/*!
    \qmlproperty enumeration StencilOperationArguments::stencilTestFailureOperation
    Holds the stencil test operation for when the stencil test fails.
    Default is StencilOperationArguments.Keep.
    \list
    \li StencilOperationArguments.Zero
    \li StencilOperationArguments.Keep
    \li StencilOperationArguments.Replace
    \li StencilOperationArguments.Increment
    \li StencilOperationArguments.Decrement
    \li StencilOperationArguments.IncrementWrap
    \li StencilOperationArguments.DecrementWrap
    \li StencilOperationArguments.Inverter
    \endlist
    \sa Qt3DRender::QStencilOperationArguments::Operation
*/

/*!
    \qmlproperty enumeration StencilOperationArguments::depthTestFailureOperation
    Holds the stencil test operation for when the stencil test passes, but
    depth test fails. Default is StencilOperationArguments.Keep.
    \sa StencilOperationArguments::stencilTestFailureOperation, Qt3DRender::QStencilOperationArguments::Operation
*/

/*!
    \qmlproperty enumeration StencilOperationArguments::allTestsPassOperation
    Holds the stencil test operation for when depth and stencil test pass. Default is StencilOperationArguments.Keep.
    \sa StencilOperationArguments::stencilTestFailureOperation, Qt3DRender::QStencilOperationArguments::Operation
*/

/*!
    \property Qt3DRender::QStencilOperationArguments::faceMode
    Holds the faces the arguments are applied to.
    \readonly
*/

/*!
    \property Qt3DRender::QStencilOperationArguments::stencilTestFailureOperation
    Holds the stencil test operation for when the stencil test fails.
    Default is StencilOperationArguments.Keep.
*/

/*!
    \property Qt3DRender::QStencilOperationArguments::depthTestFailureOperation
    Holds the stencil test operation for when the stencil test passes, but
    depth test fails. Default is StencilOperationArguments.Keep.
*/

/*!
    \property Qt3DRender::QStencilOperationArguments::allTestsPassOperation
    Holds the stencil test operation for when depth and stencil test pass. Default is StencilOperationArguments.Keep.
*/

/*!
    The constructor creates a new QStencilOperationArguments::QStencilOperationArguments
    instance with the specified \a mode and \a parent.
 */
QStencilOperationArguments::QStencilOperationArguments(FaceMode mode, QObject *parent)
    : QObject(*new QStencilOperationArgumentsPrivate(mode), parent)
{
}

/*! \internal */
QStencilOperationArguments::~QStencilOperationArguments()
{
}

QStencilOperationArguments::FaceMode QStencilOperationArguments::faceMode() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_face;
}

void QStencilOperationArguments::setStencilTestFailureOperation(QStencilOperationArguments::Operation operation)
{
    Q_D(QStencilOperationArguments);
    if (d->m_stencilTestFailureOperation != operation) {
        d->m_stencilTestFailureOperation = operation;
        Q_EMIT stencilTestFailureOperationChanged(operation);
    }
}

QStencilOperationArguments::Operation QStencilOperationArguments::stencilTestFailureOperation() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_stencilTestFailureOperation;
}

void QStencilOperationArguments::setDepthTestFailureOperation(QStencilOperationArguments::Operation operation)
{
    Q_D(QStencilOperationArguments);
    if (d->m_depthTestFailureOperation != operation) {
        d->m_depthTestFailureOperation = operation;
        Q_EMIT depthTestFailureOperationChanged(operation);
    }
}

QStencilOperationArguments::Operation QStencilOperationArguments::depthTestFailureOperation() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_depthTestFailureOperation;
}

void QStencilOperationArguments::setAllTestsPassOperation(QStencilOperationArguments::Operation operation)
{
    Q_D(QStencilOperationArguments);
    if (d->m_allTestsPassOperation != operation) {
        d->m_allTestsPassOperation = operation;
        Q_EMIT allTestsPassOperationChanged(operation);
    }
}

QStencilOperationArguments::Operation QStencilOperationArguments::allTestsPassOperation() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_allTestsPassOperation;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qstenciloperationarguments.cpp"
