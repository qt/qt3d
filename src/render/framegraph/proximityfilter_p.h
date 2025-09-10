// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_PROXIMITYFILTER_P_H
#define QT3DRENDER_RENDER_PROXIMITYFILTER_P_H

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

#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT ProximityFilter : public FrameGraphNode
{
public:
    ProximityFilter();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    float distanceThreshold() const { return m_distanceThreshold; }
    Qt3DCore::QNodeId entityId() const { return m_entityId; }

#if defined(QT_BUILD_INTERNAL)
    // For unit testing
    void setDistanceThreshold(float distanceThreshold) { m_distanceThreshold = distanceThreshold; }
    void setEntityId(Qt3DCore::QNodeId entityId) { m_entityId = entityId; }
#endif

private:
    float m_distanceThreshold;
    Qt3DCore::QNodeId m_entityId;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PROXIMITYFILTER_P_H
