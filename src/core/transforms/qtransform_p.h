/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QT3DCORE_QTRANSFORM_P_H
#define QT3DCORE_QTRANSFORM_P_H

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

#include <Qt3DCore/private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORE_PRIVATE_EXPORT QTransformPrivate : public QComponentPrivate
{
    Q_DECLARE_PUBLIC(QTransform)

public:
    QTransformPrivate();
    ~QTransformPrivate();

    // Stored in this order as QQuaternion is bigger than QVector3D
    // Operations are applied in the order of:
    // scale, rotation, translation
    QQuaternion m_rotation;
    QVector3D m_scale;
    QVector3D m_translation;
    QVector3D m_eulerRotationAngles;

    mutable QMatrix4x4 m_matrix;
    mutable bool m_matrixDirty;

    QMatrix4x4 m_worldMatrix;

    bool m_dirty;

    void setWorldMatrix(const QMatrix4x4 &worldMatrix);
    void update() override;
};

struct QTransformData
{
    QQuaternion rotation;
    QVector3D scale;
    QVector3D translation;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QTRANSFORM_P_H
