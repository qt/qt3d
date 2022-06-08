// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RAYCASTER_P_H
#define QT3DRENDER_RENDER_RAYCASTER_P_H

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
#include <Qt3DRender/qabstractraycaster.h>
#include <Qt3DRender/private/qabstractraycaster_p.h>


QT_BEGIN_NAMESPACE

#include <QtGui/QVector3D>

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT RayCaster : public BackendNode
{
public:
    RayCaster();
    ~RayCaster();

    QAbstractRayCasterPrivate::RayCasterType type() const;
    QAbstractRayCaster::RunMode runMode() const;
    QVector3D origin() const;
    QVector3D direction() const;
    float length() const;
    QPoint position() const;

    Qt3DCore::QNodeIdVector layerIds() const;
    QAbstractRayCaster::FilterMode filterMode() const;

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void cleanup();

private:
    void notifyJob();

    QAbstractRayCasterPrivate::RayCasterType m_type = QAbstractRayCasterPrivate::WorldSpaceRayCaster;
    QAbstractRayCaster::RunMode m_runMode = QAbstractRayCaster::SingleShot;
    QVector3D m_origin;
    QVector3D m_direction = {0.f, 0.f, 1.f};
    float m_length = 1.f;
    QPoint m_position;
    Qt3DCore::QNodeIdVector m_layerIds;
    QAbstractRayCaster::FilterMode m_filterMode = QAbstractRayCaster::AcceptAnyMatchingLayers;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RAYCASTER_P_H
