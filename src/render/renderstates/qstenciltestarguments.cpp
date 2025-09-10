// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qstenciltestarguments.h"
#include "qstenciltestarguments_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QStencilTestArguments
    \brief The QStencilTestArguments class specifies arguments for stencil test.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    The Qt3DRender::QStencilTestArguments class specifies the arguments for
    the stencil test.
 */

/*!
    \qmltype StencilTestArguments
    \brief The StencilTestArguments type specifies arguments for stencil test.
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QStencilTestArguments
    \inherits QtObject

    The StencilTestArguments type specifies the arguments for
    the stencil test.
 */

/*!
    \enum Qt3DRender::QStencilTestArguments::StencilFaceMode
    This enumeration holds the values for stencil test arguments face modes
    \value Front Arguments are applied to front-facing polygons.
    \value Back Arguments are applied to back-facing polygons.
    \value FrontAndBack Arguments are applied to both front- and back-facing polygons.
*/

/*!
    \enum Qt3DRender::QStencilTestArguments::StencilFunction

    Enumeration for the stencil function values
    \value Never Never pass stencil test
    \value Always Always pass stencil test
    \value Less Pass stencil test if fragment stencil is less than reference value
    \value LessOrEqual Pass stencil test if fragment stencil is less than or equal to reference value
    \value Equal Pass stencil test if fragment stencil is equal to reference value
    \value GreaterOrEqual Pass stencil test if fragment stencil is greater than or equal to reference value
    \value Greater Pass stencil test if fragment stencil is greater than reference value
    \value NotEqual Pass stencil test if fragment stencil is not equal to reference value
*/

/*!
    \qmlproperty enumeration StencilTestArguments::faceMode
    Holds the faces the arguments are applied to.
    \list
    \li StencilTestArguments.Front
    \li StencilTestArguments.Back
    \li StencilTestArguments.FrontAndBack
    \endlist
    \sa Qt3DRender::QStencilTestArguments::StencilFaceMode
    \readonly
*/

/*!
    \qmlproperty int StencilTestArguments::comparisonMask
    Holds the stencil test comparison mask. Default is all zeroes.
*/

/*!
    \qmlproperty int StencilTestArguments::referenceValue
    Holds the stencil test reference value. Default is zero.
*/

/*!
    \qmlproperty enumeration StencilTestArguments::stencilFunction
    Holds the stencil test function. Default is StencilTestArguments.Never.
    \list
    \li StencilTestArguments.Never
    \li StencilTestArguments.Always
    \li StencilTestArguments.Less
    \li StencilTestArguments.LessOrEqual
    \li StencilTestArguments.Equal
    \li StencilTestArguments.GreaterOrEqual
    \li StencilTestArguments.Greater
    \li StencilTestArguments.NotEqual
    \endlist
*/

/*!
    \property Qt3DRender::QStencilTestArguments::faceMode
    Holds the faces the arguments are applied to.
    \readonly
*/

/*!
    \property Qt3DRender::QStencilTestArguments::comparisonMask
    Holds the stencil test comparison mask. Default is all zeroes.
*/

/*!
    \property Qt3DRender::QStencilTestArguments::referenceValue
    Holds the stencil test reference value. Default is zero.
*/

/*!
    \property Qt3DRender::QStencilTestArguments::stencilFunction
    Holds the stencil test function. Default is Never.
    \sa Qt3DRender::QStencilTestArguments::StencilFunction
*/

/*!
    The constructor creates a new QStencilTestArguments::QStencilTestArguments
    instance with the specified \a face and \a parent.
 */
QStencilTestArguments::QStencilTestArguments(QStencilTestArguments::StencilFaceMode face, QObject *parent)
    : QObject(*new QStencilTestArgumentsPrivate(face), parent)
{
}

/*! \internal */
QStencilTestArguments::~QStencilTestArguments()
{
}

uint QStencilTestArguments::comparisonMask() const
{
    Q_D(const QStencilTestArguments);
    return d->m_comparisonMask;
}

void QStencilTestArguments::setComparisonMask(uint comparisonMask)
{
    Q_D(QStencilTestArguments);
    if (d->m_comparisonMask != comparisonMask) {
        d->m_comparisonMask = comparisonMask;
        emit comparisonMaskChanged(comparisonMask);
    }
}

int QStencilTestArguments::referenceValue() const
{
    Q_D(const QStencilTestArguments);
    return d->m_referenceValue;
}

void QStencilTestArguments::setReferenceValue(int referenceValue)
{
    Q_D(QStencilTestArguments);
    if (d->m_referenceValue != referenceValue) {
        d->m_referenceValue = referenceValue;
        emit referenceValueChanged(referenceValue);
    }
}

QStencilTestArguments::StencilFunction QStencilTestArguments::stencilFunction() const
{
    Q_D(const QStencilTestArguments);
    return d->m_stencilFunction;
}

void QStencilTestArguments::setStencilFunction(QStencilTestArguments::StencilFunction stencilFunction)
{
    Q_D(QStencilTestArguments);
    if (d->m_stencilFunction != stencilFunction) {
        d->m_stencilFunction = stencilFunction;
        emit stencilFunctionChanged(stencilFunction);
    }
}

QStencilTestArguments::StencilFaceMode QStencilTestArguments::faceMode() const
{
    Q_D(const QStencilTestArguments);
    return d->m_face;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qstenciltestarguments.cpp"
