// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_RHIHANDLE_TYPES_P_H
#define QT3DRENDER_RENDER_RHI_RHIHANDLE_TYPES_P_H

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

#include <Qt3DCore/private/qhandle_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class RHIBuffer;
class RHITexture;
class RHIGraphicsPipeline;
class RHIComputePipeline;
struct RHIRenderTarget;

typedef Qt3DCore::QHandle<RHIBuffer> HRHIBuffer;
typedef Qt3DCore::QHandle<RHITexture> HRHITexture;
typedef Qt3DCore::QHandle<RHIGraphicsPipeline> HRHIGraphicsPipeline;
typedef Qt3DCore::QHandle<RHIComputePipeline> HRHIComputePipeline;
typedef Qt3DCore::QHandle<RHIRenderTarget> HRHIRenderTarget;

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

#if defined(_MSC_VER)
#define RHI_UNIMPLEMENTED // do { qDebug() << "Unimplemented: " << __FUNCSIG__; } while (0)
#else
#define RHI_UNIMPLEMENTED // do { qDebug() << "Unimplemented: " << __PRETTY_FUNCTION__; } while (0)
#endif
QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIHANDLE_TYPES_P_H
