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
****************************************************************************/

#ifndef QT3DRENDER_CAMERA_P_H
#define QT3DRENDER_CAMERA_P_H

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

#include <Qt3DRender/qcameralens.h>
#include <Qt3DCore/qtransform.h>
#include <private/qentity_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QCameraPrivate : public Qt3DCore::QEntityPrivate
{
public:
    QCameraPrivate();

    Q_DECLARE_PUBLIC(QCamera)

    void updateViewMatrixAndTransform(bool doEmit = true);

    QVector3D m_position;
    QVector3D m_viewCenter;
    QVector3D m_upVector;

    QVector3D m_cameraToCenter; // The vector from the camera position to the view center
    bool m_viewMatrixDirty;

    // Components
    QCameraLens *m_lens;
    Qt3DCore::QTransform *m_transform;
    QMatrix4x4 m_viewMatrix;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_CAMERA_P_H
