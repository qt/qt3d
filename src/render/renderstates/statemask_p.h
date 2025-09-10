// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_STATEMASK_P_H
#define QT3DRENDER_RENDER_STATEMASK_P_H

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

#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

enum StateMask
{
    BlendStateMask          = 1 << 0,
    StencilWriteStateMask   = 1 << 1,
    StencilTestStateMask    = 1 << 2,
    ScissorStateMask        = 1 << 3,
    DepthTestStateMask      = 1 << 4,
    DepthWriteStateMask     = 1 << 5,
    CullFaceStateMask       = 1 << 6,
    AlphaTestMask           = 1 << 7,
    FrontFaceStateMask      = 1 << 8,
    DitheringStateMask      = 1 << 9,
    AlphaCoverageStateMask  = 1 << 10,
    PolygonOffsetStateMask  = 1 << 11,
    ColorStateMask          = 1 << 12,
    ClipPlaneMask           = 1 << 13,
    StencilOpMask           = 1 << 14,
    PointSizeMask           = 1 << 15,
    SeamlessCubemapMask     = 1 << 16,
    MSAAEnabledStateMask    = 1 << 17,
    BlendEquationArgumentsMask  = 1 << 18,
    LineWidthMask           = 1 << 19,
    DepthRangeMask          = 1 << 20,
    RasterModeMask          = 1 << 21
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_STATEMASK_P_H
