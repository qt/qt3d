/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "transform_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qtransform.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Transform::Transform()
    : QBackendNode()
    , m_rotation()
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_translation()
{
}

void Transform::updateFromPeer(Qt3DCore::QNode *peer)
{
    Qt3DCore::QTransform *transform = static_cast<Qt3DCore::QTransform *>(peer);

    m_rotation = transform->rotation();
    m_scale = transform->scale3D();
    m_translation = transform->translation();
    updateMatrix();
    m_enabled = transform->isEnabled();
}

QMatrix4x4 Transform::transformMatrix() const
{
    return m_transformMatrix;
}

void Transform::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    // TODO: Flag the matrix as dirty and update all matrices batched in a job
    if (e->type() == NodeUpdated) {
        const QScenePropertyChangePtr &propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("scale3D")) {
            m_scale = propertyChange->value().value<QVector3D>();
            updateMatrix();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("rotation")) {
            m_rotation = propertyChange->value().value<QQuaternion>();
            updateMatrix();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("translation")) {
            m_translation = propertyChange->value().value<QVector3D>();
            updateMatrix();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            m_enabled = propertyChange->value().toBool();
        }
    }
}

void Transform::updateMatrix()
{
    QMatrix4x4 m;
    m.translate(m_translation);
    m.rotate(m_rotation);
    m.scale(m_scale);
    m_transformMatrix = m;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
