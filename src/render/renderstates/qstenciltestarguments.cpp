/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include "qstenciltestarguments.h"
#include "qstenciltestarguments_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class QStencilTestArguments
 * \brief The QStencilTestArguments class
 * \since 5.7
 * \ingroup renderstates
 */

/*!
 * The constructor creates a new QStencilTestArguments::QStencilTestArguments
 * instance with the specified \a face and \a parent.
 */
QStencilTestArguments::QStencilTestArguments(QStencilTestArguments::StencilFaceMode face, QObject *parent)
    : QObject(*new QStencilTestArgumentsPrivate(face), parent)
{
}

/*! \internal */
QStencilTestArguments::~QStencilTestArguments()
{
}

/*!
 * \return the current comparison mask.
 */
uint QStencilTestArguments::comparisonMask() const
{
    Q_D(const QStencilTestArguments);
    return d->m_comparisonMask;
}

/*!
 * Sets the comparison mask.
 * \param comparisonMask
 */
void QStencilTestArguments::setComparisonMask(uint comparisonMask)
{
    Q_D(QStencilTestArguments);
    if (d->m_comparisonMask != comparisonMask) {
        d->m_comparisonMask = comparisonMask;
        emit comparisonMaskChanged(comparisonMask);
    }
}

/*!
 * \return the current reference value.
 */
int QStencilTestArguments::referenceValue() const
{
    Q_D(const QStencilTestArguments);
    return d->m_referenceValue;
}

/*!
 * Sets the reference value.
 * \param referenceValue
 */
void QStencilTestArguments::setReferenceValue(int referenceValue)
{
    Q_D(QStencilTestArguments);
    if (d->m_referenceValue != referenceValue) {
        d->m_referenceValue = referenceValue;
        emit referenceValueChanged(referenceValue);
    }
}

/*!
 * \return the current stencil function.
 */
QStencilTestArguments::StencilFunction QStencilTestArguments::stencilFunction() const
{
    Q_D(const QStencilTestArguments);
    return d->m_stencilFunction;
}

/*!
 * Sets the stencil function.
 * \param stencilFunction
 */
void QStencilTestArguments::setStencilFunction(QStencilTestArguments::StencilFunction stencilFunction)
{
    Q_D(QStencilTestArguments);
    if (d->m_stencilFunction != stencilFunction) {
        d->m_stencilFunction = stencilFunction;
        emit stencilFunctionChanged(stencilFunction);
    }
}

/*!
 * \return the current face mode.
 */
QStencilTestArguments::StencilFaceMode QStencilTestArguments::faceMode() const
{
    Q_D(const QStencilTestArguments);
    return d->m_face;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
