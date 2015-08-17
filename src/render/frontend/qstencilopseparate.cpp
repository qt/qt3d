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

#include "qstencilopseparate.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QStencilOpSeparatePrivate : public QObjectPrivate
{
public:
    QStencilOpSeparatePrivate(QStencilOpSeparate::StencilFaceMode mode)
        : QObjectPrivate()
        , m_face(mode)
        , m_stencilFailOp(QStencilOpSeparate::Keep)
        , m_depthFailOp(QStencilOpSeparate::Keep)
        , m_stencilDepthPassOp(QStencilOpSeparate::Keep)
    {

    }

    QStencilOpSeparate::StencilFaceMode m_face;
    QStencilOpSeparate::StencilOp m_stencilFailOp;
    QStencilOpSeparate::StencilOp m_depthFailOp;
    QStencilOpSeparate::StencilOp m_stencilDepthPassOp;
};

QStencilOpSeparate::QStencilOpSeparate(StencilFaceMode mode, QObject *parent)
    : QObject(*new QStencilOpSeparatePrivate(mode), parent)
{
}

QStencilOpSeparate::~QStencilOpSeparate()
{
}

QStencilOpSeparate::StencilFaceMode QStencilOpSeparate::faceMode() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_face;
}

void QStencilOpSeparate::setStencilFail(QStencilOpSeparate::StencilOp op)
{
    Q_D(QStencilOpSeparate);
    if (d->m_stencilFailOp != op) {
        d->m_stencilFailOp = op;
        Q_EMIT stencilFailChanged();
    }
}

QStencilOpSeparate::StencilOp QStencilOpSeparate::stencilFail() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_stencilFailOp;
}

void QStencilOpSeparate::setDepthFail(QStencilOpSeparate::StencilOp op)
{
    Q_D(QStencilOpSeparate);
    if (d->m_depthFailOp != op) {
        d->m_depthFailOp = op;
        Q_EMIT depthFailChanged();
    }
}

QStencilOpSeparate::StencilOp QStencilOpSeparate::depthFail() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_depthFailOp;
}

void QStencilOpSeparate::setStencilDepthPass(QStencilOpSeparate::StencilOp op)
{
    Q_D(QStencilOpSeparate);
    if (d->m_stencilDepthPassOp != op) {
        d->m_stencilDepthPassOp = op;
        Q_EMIT stencilDepthPassChanged();
    }
}

QStencilOpSeparate::StencilOp QStencilOpSeparate::stencilDepthPass() const
{
    Q_D(const QStencilOpSeparate);
    return d->m_stencilDepthPassOp;
}

} // Qt3D

QT_END_NAMESPACE
