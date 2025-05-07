// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "rhirendertarget_p.h"
QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

void RHIRenderTarget::cleanup()
{
    delete renderPassDescriptor;
    delete renderTarget;
    delete depthStencilBuffer;
    renderPassDescriptor = nullptr;
    renderTarget = nullptr;
    depthStencilBuffer = nullptr;
}

RHIRenderTarget::~RHIRenderTarget()
{
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
