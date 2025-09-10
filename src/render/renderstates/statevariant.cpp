// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "statevariant_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

RenderStateImpl *StateVariant::state()
{
    switch (type) {
    case BlendEquationArgumentsMask:
    case BlendStateMask:
    case AlphaTestMask:
    case MSAAEnabledStateMask:
    case DepthRangeMask:
    case DepthTestStateMask:
    case DepthWriteStateMask:
    case CullFaceStateMask:
    case FrontFaceStateMask:
    case DitheringStateMask:
    case ScissorStateMask:
    case StencilTestStateMask:
    case AlphaCoverageStateMask:
    case PointSizeMask:
    case PolygonOffsetStateMask:
    case ColorStateMask:
    case ClipPlaneMask:
    case SeamlessCubemapMask:
    case StencilOpMask:
    case StencilWriteStateMask:
    case LineWidthMask:
    case RasterModeMask:
        return &data.blendEquationArguments;
    default:
        Q_UNREACHABLE();
    }
}

const RenderStateImpl *StateVariant::constState() const
{
    switch (type) {
    case BlendEquationArgumentsMask:
    case BlendStateMask:
    case AlphaTestMask:
    case MSAAEnabledStateMask:
    case DepthRangeMask:
    case DepthTestStateMask:
    case DepthWriteStateMask:
    case CullFaceStateMask:
    case FrontFaceStateMask:
    case DitheringStateMask:
    case ScissorStateMask:
    case StencilTestStateMask:
    case AlphaCoverageStateMask:
    case PointSizeMask:
    case PolygonOffsetStateMask:
    case ColorStateMask:
    case ClipPlaneMask:
    case SeamlessCubemapMask:
    case StencilOpMask:
    case StencilWriteStateMask:
    case LineWidthMask:
    case RasterModeMask:
        return &data.blendEquationArguments;
    default:
        Q_UNREACHABLE();
    }
}

bool StateVariant::operator ==(const StateVariant &other) const
{
    return (other.type == type && constState()->equalTo(*other.constState()));
}

bool StateVariant::operator !=(const StateVariant &other) const
{
    return !(*this == other);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
