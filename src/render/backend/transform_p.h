// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_TRANSFORM_H
#define QT3DRENDER_RENDER_TRANSFORM_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/backendnode_p.h>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>
#include <Qt3DCore/private/matrix4x4_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Renderer;
class TransformManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Transform : public BackendNode
{
public:
    Transform();
    void cleanup();

    Matrix4x4 transformMatrix() const;
    bool hasViewMatrix() const { return m_hasViewMatrix; }
    const Matrix4x4& viewMatrix() const { return m_viewMatrix; }
    QVector3D scale() const;
    QQuaternion rotation() const;
    QVector3D translation() const;

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) final;

private:
    void updateMatrix();
    bool m_hasViewMatrix;
    Matrix4x4 m_viewMatrix;
    Matrix4x4 m_transformMatrix;
    QQuaternion m_rotation;
    QVector3D m_scale;
    QVector3D m_translation;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_TRANSFORM_H
