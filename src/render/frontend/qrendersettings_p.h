// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERSETTINGS_P_H
#define QT3DRENDER_QRENDERSETTINGS_P_H

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
#include <Qt3DRender/qrendersettings.h>
#include <Qt3DRender/qpickingsettings.h>
#include <Qt3DRender/qrendercapabilities.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QRenderSettingsPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QRenderSettingsPrivate();

    void init();

    QPickingSettings m_pickingSettings;
    QFrameGraphNode *m_activeFrameGraph;
    QRenderSettings::RenderPolicy m_renderPolicy;
    QRenderCapabilities m_renderCapabilities;

    void invalidateFrame();

    void _q_onPickingMethodChanged(QPickingSettings::PickMethod pickMethod);
    void _q_onPickResultModeChanged(QPickingSettings::PickResultMode pickResultMode);
    void _q_onFaceOrientationPickingModeChanged(QPickingSettings::FaceOrientationPickingMode faceOrientationPickingMode);
    void _q_onWorldSpaceToleranceChanged(float worldSpaceTolerance);

    Q_DECLARE_PUBLIC(QRenderSettings)
};

struct QRenderSettingsData
{
    Qt3DCore::QNodeId activeFrameGraphId;
    QRenderSettings::RenderPolicy renderPolicy;
    QPickingSettings::PickMethod pickMethod;
    QPickingSettings::PickResultMode pickResultMode;
    QPickingSettings::FaceOrientationPickingMode faceOrientationPickingMode;
    float pickWorldSpaceTolerance;
};

} // namespace Qt3Drender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERSETTINGS_P_H
