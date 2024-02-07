// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "transform_p.h"

#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/private/qtransform_p.h>
#include <Qt3DRender/qcamera.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

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
        auto camera = qobject_cast<const Qt3DRender::QCamera *>(transform->parentNode());
        if (camera) {
            m_viewMatrix = Matrix4x4(camera->viewMatrix());
            m_hasViewMatrix = true;
        } else {
            m_hasViewMatrix = false;
        }
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
