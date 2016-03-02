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

#include "qstenciloperation.h"
#include "qstencilopseparate.h"
#include <Qt3DRender/private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QStencilOperationPrivate : public QRenderStatePrivate
{
public:
    QStencilOperationPrivate()
        : QRenderStatePrivate(QRenderState::StencilOp)
        , m_front(new QStencilOpSeparate(QStencilOpSeparate::Front, q_ptr))
        , m_back(new QStencilOpSeparate(QStencilOpSeparate::Back, q_ptr))
    {}

    QStencilOpSeparate *m_front;
    QStencilOpSeparate *m_back;
};


QStencilOperation::QStencilOperation(QNode *parent)
    : QRenderState(*new QStencilOperationPrivate(), parent)
{
}

QStencilOperation::~QStencilOperation()
{
    QNode::cleanup();
}

QStencilOpSeparate *QStencilOperation::front() const
{
    Q_D(const QStencilOperation);
    return d->m_front;
}

QStencilOpSeparate *QStencilOperation::back() const
{
    Q_D(const QStencilOperation);
    return d->m_back;
}

void QStencilOperation::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QStencilOperation *refState = static_cast<const QStencilOperation*>(ref);
    d_func()->m_back->setDepthFail(refState->d_func()->m_back->depthFail());
    d_func()->m_back->setStencilFail(refState->d_func()->m_back->stencilFail());
    d_func()->m_back->setStencilDepthPass(refState->d_func()->m_back->stencilDepthPass());
    d_func()->m_front->setDepthFail(refState->d_func()->m_front->depthFail());
    d_func()->m_front->setStencilFail(refState->d_func()->m_front->stencilFail());
    d_func()->m_front->setStencilDepthPass(refState->d_func()->m_front->stencilDepthPass());
}

} // namespace Qt3DRender

QT_END_NAMESPACE
