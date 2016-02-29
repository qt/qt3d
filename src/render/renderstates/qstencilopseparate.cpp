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

#include "qstencilopseparate.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilOpSeparatePrivate : public QObjectPrivate
{
public:
    QStencilOpSeparatePrivate(QStencilOpSeparate::FaceMode mode)
        : QObjectPrivate()
        , m_face(mode)
        , m_stencilTestFailureOperation(QStencilOpSeparate::Keep)
        , m_depthTestFailureOperation(QStencilOpSeparate::Keep)
        , m_allTestsPassOperation(QStencilOpSeparate::Keep)
    {

    }

    QStencilOpSeparate::FaceMode m_face;
    QStencilOpSeparate::Operation m_stencilTestFailureOperation;
    QStencilOpSeparate::Operation m_depthTestFailureOperation;
    QStencilOpSeparate::Operation m_allTestsPassOperation;
};

QStencilOpSeparate::QStencilOpSeparate(FaceMode mode, QObject *parent)
    : QObject(*new QStencilOpSeparatePrivate(mode), parent)
{
}

QStencilOpSeparate::~QStencilOpSeparate()
{
}

QStencilOpSeparate::FaceMode QStencilOpSeparate::faceMode() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_face;
}

void QStencilOpSeparate::setStencilTestFailureOperation(QStencilOpSeparate::Operation operation)
{
    Q_D(QStencilOpSeparate);
    if (d->m_stencilTestFailureOperation != operation) {
        d->m_stencilTestFailureOperation = operation;
        Q_EMIT stencilTestFailureOperationChanged(operation);
    }
}

QStencilOpSeparate::Operation QStencilOpSeparate::stencilTestFailureOperation() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_stencilTestFailureOperation;
}

void QStencilOpSeparate::setDepthTestFailureOperation(QStencilOpSeparate::Operation operation)
{
    Q_D(QStencilOpSeparate);
    if (d->m_depthTestFailureOperation != operation) {
        d->m_depthTestFailureOperation = operation;
        Q_EMIT depthTestFailureOperationChanged(operation);
    }
}

QStencilOpSeparate::Operation QStencilOpSeparate::depthTestFailureOperation() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_depthTestFailureOperation;
}

void QStencilOpSeparate::setAllTestsPassOperation(QStencilOpSeparate::Operation operation)
{
    Q_D(QStencilOpSeparate);
    if (d->m_allTestsPassOperation != operation) {
        d->m_allTestsPassOperation = operation;
        Q_EMIT allTestsPassOperationChanged(operation);
    }
}

QStencilOpSeparate::Operation QStencilOpSeparate::allTestsPassOperation() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_allTestsPassOperation;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
