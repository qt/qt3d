// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QABSTRACTRAYCASTER_P_H
#define QT3DRENDER_QABSTRACTRAYCASTER_P_H

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
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/QAbstractRayCaster>
#include <Qt3DRender/private/qt3drender_global_p.h>

#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLayer;

/*!
    \internal
*/
class Q_3DRENDERSHARED_PRIVATE_EXPORT QAbstractRayCasterPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QAbstractRayCasterPrivate();

    enum RayCasterType {
        WorldSpaceRayCaster,
        ScreenScapeRayCaster
    };

    static QAbstractRayCasterPrivate *get(QAbstractRayCaster *obj);
    static const QAbstractRayCasterPrivate *get(const QAbstractRayCaster *obj);
    static void updateHitEntites(QAbstractRayCaster::Hits &hits, Qt3DCore::QScene *scene);

    RayCasterType m_rayCasterType = WorldSpaceRayCaster;
    QAbstractRayCaster::RunMode m_runMode = QAbstractRayCaster::SingleShot;
    QAbstractRayCaster::Hits m_hits;
    QPoint m_position;
    QVector3D m_origin;
    QVector3D m_direction = QVector3D(0., 0., 1.f);
    float m_length = 1.f;
    QAbstractRayCaster::FilterMode m_filterMode = QAbstractRayCaster::AcceptAnyMatchingLayers;
    QList<QLayer *> m_layers;

    QAbstractRayCaster::Hits pick();
    virtual void dispatchHits(const QAbstractRayCaster::Hits &hits);

    Q_DECLARE_PUBLIC(QAbstractRayCaster)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QABSTRACTRAYCASTER_P_H
