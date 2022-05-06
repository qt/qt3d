/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#ifndef QT3DRENDER_RENDER_JOB_COMMON_P_H
#define QT3DRENDER_RENDER_JOB_COMMON_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qaspectjob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace JobTypes {

    enum JobType {
        LoadBuffer = 1,
        FrameCleanup,
        UpdateShaderDataTransform,
        CalcBoundingVolume,
        CalcTriangleVolume,
        LoadGeometry,
        LoadScene,
        LoadTextureData,
        PickBoundingVolume,
        RayCasting,
        RenderView,
        UpdateTransform,
        UpdateTreeEnabled,
        ExpandBoundingVolume,
        FrameSubmissionPart1,
        LayerFiltering,
        EntityComponentTypeFiltering,
        MaterialParameterGathering,
        RenderCommandUpdater,
        GenericLambda,
        FrustumCulling,
        LightGathering,
        UpdateWorldBoundingVolume,
        FrameSubmissionPart2,
        DirtyBufferGathering,
        DirtyVaoGathering,
        DirtyTextureGathering,
        DirtyShaderGathering,
        SendRenderCapture,
        SendBufferCapture,
        SyncRenderViewPreCommandUpdate,
        SyncRenderViewInitialization,
        SyncRenderViewPostCommandUpdate,
        SyncFrustumCulling,
        ClearBufferDrawIndex,
        UpdateMeshTriangleList,
        FilterCompatibleTechniques,
        UpdateLevelOfDetail,
        SyncLoadingJobs,
        LoadSkeleton,
        UpdateSkinningPalette,
        ProximityFiltering,
        SyncFilterEntityByLayer,
        SyncMaterialGatherer,
        UpdateLayerEntity,
        SendTextureChangesToFrontend,
        SendSetFenceHandlesToFrontend,
        SendDisablesToFrontend,
        RenderViewCommandBuilder,
        SyncRenderViewPreCommandBuilding
    };

} // JobTypes

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_JOB_COMMON_P_H
