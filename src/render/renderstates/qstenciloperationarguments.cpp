/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
 * \class QStencilOperationArguments
 * \brief The QStencilOperationArguments class sets the actions to be taken
 * when stencil and depth tests fail.
 * \since 5.7
 * \ingroup renderstates
 *
 */

#include "qstenciloperationarguments.h"
#include "qstenciloperationarguments_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * The constructor creates a new QStencilOperationArguments::QStencilOperationArguments
 * instance with the specified \a mode and \a parent.
 */
QStencilOperationArguments::QStencilOperationArguments(FaceMode mode, QObject *parent)
    : QObject(*new QStencilOperationArgumentsPrivate(mode), parent)
{
}

/*! \internal */
QStencilOperationArguments::~QStencilOperationArguments()
{
}

/*!
 * \return the current face mode.
 */
QStencilOperationArguments::FaceMode QStencilOperationArguments::faceMode() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_face;
}

/*!
 * Sets the stencil action when a test fails, to operation.
 * \param operation
 */
void QStencilOperationArguments::setStencilTestFailureOperation(QStencilOperationArguments::Operation operation)
{
    Q_D(QStencilOperationArguments);
    if (d->m_stencilTestFailureOperation != operation) {
        d->m_stencilTestFailureOperation = operation;
        Q_EMIT stencilTestFailureOperationChanged(operation);
    }
}

/*!
 * \return the current stencil test failure operation.
 */
QStencilOperationArguments::Operation QStencilOperationArguments::stencilTestFailureOperation() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_stencilTestFailureOperation;
}

/*!
 * Sets the action when the depth test fails, to operation.
 * \param operation
 */
void QStencilOperationArguments::setDepthTestFailureOperation(QStencilOperationArguments::Operation operation)
{
    Q_D(QStencilOperationArguments);
    if (d->m_depthTestFailureOperation != operation) {
        d->m_depthTestFailureOperation = operation;
        Q_EMIT depthTestFailureOperationChanged(operation);
    }
}

/*!
 * \return the current depth test failure operation.
 */
QStencilOperationArguments::Operation QStencilOperationArguments::depthTestFailureOperation() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_depthTestFailureOperation;
}

/*!
 * Sets the action when both the depth and stencil tests fail, to operation.
 * \param operation
 */
void QStencilOperationArguments::setAllTestsPassOperation(QStencilOperationArguments::Operation operation)
{
    Q_D(QStencilOperationArguments);
    if (d->m_allTestsPassOperation != operation) {
        d->m_allTestsPassOperation = operation;
        Q_EMIT allTestsPassOperationChanged(operation);
    }
}

/*!
 * \return the current failure operation for when both tests fail.
 */
QStencilOperationArguments::Operation QStencilOperationArguments::allTestsPassOperation() const
{
    Q_D(const QStencilOperationArguments);
    return d->m_allTestsPassOperation;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
