// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LEVELOFDETAIL_H
#define QT3DRENDER_RENDER_LEVELOFDETAIL_H

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
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qlevelofdetail.h>
#include <QStringList>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QLevelOfDetail;

namespace Render {

class LevelOfDetailManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT LevelOfDetail : public BackendNode
{
public:
    LevelOfDetail();
    ~LevelOfDetail();
    void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    Qt3DCore::QNodeId camera() const { return m_camera; }
    int currentIndex() const { return m_currentIndex; }
    QLevelOfDetail::ThresholdType thresholdType() const { return m_thresholdType; }
    QList<qreal> thresholds() const { return m_thresholds; }
    float radius() const { return m_volumeOverride.radius(); }
    QVector3D center() const { return m_volumeOverride.center(); }
    bool hasBoundingVolumeOverride() const { return !m_volumeOverride.isEmpty(); }

    void setCurrentIndex(int currentIndex);

private:
    Qt3DCore::QNodeId m_camera;
    int m_currentIndex;
    QLevelOfDetail::ThresholdType m_thresholdType;
    QList<qreal> m_thresholds;
    QLevelOfDetailBoundingSphere m_volumeOverride;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_LEVELOFDETAIL_H
