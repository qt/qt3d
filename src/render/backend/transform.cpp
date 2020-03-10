/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "transform_p.h"

#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qtransform_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Transform::Transform()
    : BackendNode(ReadWrite)
    , m_rotation()
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_translation()
{
}

void Transform::cleanup()
{
    m_rotation = QQuaternion();
    m_scale = QVector3D();
    m_translation = QVector3D();
    m_transformMatrix = Matrix4x4();
    QBackendNode::setEnabled(false);
}

Matrix4x4 Transform::transformMatrix() const
{
    return m_transformMatrix;
}

QVector3D Transform::scale() const
{
    return m_scale;
}

QQuaternion Transform::rotation() const
{
    return m_rotation;
}

QVector3D Transform::translation() const
{
    return m_translation;
}

void Transform::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const Qt3DCore::QTransform *transform = qobject_cast<const Qt3DCore::QTransform *>(frontEnd);
    if (!transform)
        return;

    bool dirty = m_rotation != transform->rotation();
    m_rotation = transform->rotation();
    dirty |= m_scale != transform->scale3D();
    m_scale = transform->scale3D();
    dirty |= m_translation != transform->translation();
    m_translation = transform->translation();

    if (dirty || firstTime) {
        updateMatrix();
        markDirty(AbstractRenderer::TransformDirty);
    }

    if (transform->isEnabled() != isEnabled())
        markDirty(AbstractRenderer::TransformDirty);

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
}

void Transform::updateMatrix()
{
    QMatrix4x4 m;
    m.translate(m_translation);
    m.rotate(m_rotation);
    m.scale(m_scale);
    m_transformMatrix = Matrix4x4(m);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
