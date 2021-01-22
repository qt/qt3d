/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_SQT_P_H
#define QT3DCORE_SQT_P_H

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

#include <QtGui/qmatrix4x4.h>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

struct Sqt
{
    QQuaternion rotation;
    QVector3D scale;
    float pad1;
    QVector3D translation;
    float pad2;

    Sqt()
        : rotation()
        , scale(1.0f, 1.0f, 1.0f)
        , pad1(0.f)
        , translation()
        , pad2(0.f)
    {}

    inline QMatrix4x4 toMatrix() const
    {
        QMatrix4x4 m;
        m.translate(translation);
        m.rotate(rotation);
        m.scale(scale);
        return m;
    }

    bool operator == (const Sqt &rhs) const
    {
        return rotation == rhs.rotation
                && scale == rhs.scale
                && translation == rhs.translation;
    }
};

struct JointNamesAndLocalPoses
{
    QVector<QString> names;
    QVector<Sqt> localPoses;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QVector<Qt3DCore::Sqt>)
Q_DECLARE_METATYPE(Qt3DCore::JointNamesAndLocalPoses)

#endif // QT3DCORE_SQT_P_H
