// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QBOUNDINGVOLUME_P_H
#define QT3DCORE_QBOUNDINGVOLUME_P_H

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

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QBoundingVolume;
class QGeometryView;

class Q_3DCORESHARED_EXPORT QBoundingVolumePrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QBoundingVolume)

    QBoundingVolumePrivate();

    void setImplicitBounds(const QVector3D &minPoint, const QVector3D &maxPoint, const QVector3D &center, float radius);
    virtual void setView(QGeometryView *view);

    static QBoundingVolumePrivate *get(QBoundingVolume *q);

    QGeometryView *m_view;
    QVector3D m_implicitMinPoint;
    QVector3D m_implicitMaxPoint;
    QVector3D m_implicitCenter;
    QVector3D m_minPoint;
    QVector3D m_maxPoint;
    float m_implicitRadius;
    bool m_implicitPointsValid;
    bool m_explicitPointsValid;
    bool m_primaryProvider;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBOUNDINGVOLUME_P_H
