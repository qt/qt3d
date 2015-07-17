/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qstencilop.h"
#include "qstencilopseparate.h"
#include <Qt3DRenderer/private/qrenderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QStencilOpPrivate : public QRenderStatePrivate
{
public:
    QStencilOpPrivate()
        : QRenderStatePrivate(QRenderState::StencilOp)
        , m_front(new QStencilOpSeparate(QStencilOpSeparate::Front, q_ptr))
        , m_back(new QStencilOpSeparate(QStencilOpSeparate::Back, q_ptr))
    {}

    QStencilOpSeparate *m_front;
    QStencilOpSeparate *m_back;
};


QStencilOp::QStencilOp(QNode *parent)
    : QRenderState(*new QStencilOpPrivate(), parent)
{
}

QStencilOp::~QStencilOp()
{
    QNode::cleanup();
}

QStencilOpSeparate *QStencilOp::front() const
{
    Q_D(const QStencilOp);
    return d->m_front;
}

QStencilOpSeparate *QStencilOp::back() const
{
    Q_D(const QStencilOp);
    return d->m_back;
}

void QStencilOp::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const QStencilOp *refState = static_cast<const QStencilOp*>(ref);
    d_func()->m_back->setDepthFail(refState->d_func()->m_back->depthFail());
    d_func()->m_back->setStencilFail(refState->d_func()->m_back->stencilFail());
    d_func()->m_back->setStencilDepthPass(refState->d_func()->m_back->stencilDepthPass());
    d_func()->m_front->setDepthFail(refState->d_func()->m_front->depthFail());
    d_func()->m_front->setStencilFail(refState->d_func()->m_front->stencilFail());
    d_func()->m_front->setStencilDepthPass(refState->d_func()->m_front->stencilDepthPass());
}

} // Qt3D

QT_END_NAMESPACE
