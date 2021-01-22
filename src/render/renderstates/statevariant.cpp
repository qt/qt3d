/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
