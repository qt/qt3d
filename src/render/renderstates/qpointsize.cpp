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
#include "qpointsize.h"
#include "qrenderstate_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPointSizePrivate : public QRenderStatePrivate
{
public:
    QPointSizePrivate(QPointSize::Specification spec, float value)
        : QRenderStatePrivate(QRenderState::PointSize)
        , m_specification(spec)
        , m_value(value)
    {}

    QPointSize::Specification m_specification;
    float m_value;

    Q_DECLARE_PUBLIC(QPointSize)
};

/*!
    \class Qt3DRender::QPointSize
    \inmodule Qt3DRender
    \brief Specifies the size of rasterized points. May either be set statically
    or by shader programs.

    When using StaticValue, the value is set using glPointSize(), if available.
    When using Programmable, gl_PointSize must be set within shader programs,
    the value provided to this RenderState is ignored in that case.
 */

QPointSize::QPointSize(Qt3DCore::QNode *parent)
    : QRenderState(*new QPointSizePrivate(Programmable, 0.f), parent)
{
}

QPointSize::~QPointSize()
{
    QNode::cleanup();
}

QPointSize::Specification QPointSize::specification() const
{
    Q_D(const QPointSize);
    return d->m_specification;
}

float QPointSize::value() const
{
    Q_D(const QPointSize);
    return d->m_value;
}

bool QPointSize::isProgrammable() const
{
    return (specification() == QPointSize::Programmable);
}

void QPointSize::setSpecification(Specification spec)
{
    Q_D(QPointSize);
    d->m_specification = spec;
    emit specificationChanged(spec);
}

void QPointSize::setValue(float size)
{
    Q_D(QPointSize);
    d->m_value = size;
    emit valueChanged(size);
}

void QPointSize::copy(const Qt3DCore::QNode *ref)
{
    const QPointSize *refState = static_cast<const QPointSize *>(ref);

    QRenderState::copy(ref);

    Q_D(QPointSize);
    d->m_value = refState->d_func()->m_value;
    d->m_specification = refState->d_func()->m_specification;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

