// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RHIRENDERTARGET_H
#define QT3DRENDER_RENDER_RHI_RHIRENDERTARGET_H

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

#include <rhi/qrhi.h>
#include <rhihandle_types_p.h>
#include <private/attachmentpack_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
namespace Render {
namespace Rhi {

struct RHIRenderTarget {
    // Note: only used when rendering to a FBO
    // as we don't want to remove the default framebuffer's rendertarget
    // TODO: at some point separate render target and graphics pipeline as the former can be reused
    QRhiRenderTarget *renderTarget{};
    QRhiRenderPassDescriptor *renderPassDescriptor{};
    QRhiRenderBuffer *depthStencilBuffer{};

    ~RHIRenderTarget();
    void cleanup();
};

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RENDERER_H
