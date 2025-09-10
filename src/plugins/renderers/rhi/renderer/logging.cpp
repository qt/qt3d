// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "logging_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

Q_LOGGING_CATEGORY(Backend, "Qt3D.Renderer.RHI.Backend", QtWarningMsg)
Q_LOGGING_CATEGORY(Frontend, "Qt3D.Renderer.RHI.Frontend", QtWarningMsg)
Q_LOGGING_CATEGORY(Io, "Qt3D.Renderer.RHI.IO", QtWarningMsg)
Q_LOGGING_CATEGORY(Jobs, "Qt3D.Renderer.RHI.Jobs", QtWarningMsg)
Q_LOGGING_CATEGORY(SceneLoaders, "Qt3D.Renderer.RHI.SceneLoaders", QtWarningMsg)
Q_LOGGING_CATEGORY(Framegraph, "Qt3D.Renderer.RHI.Framegraph", QtWarningMsg)
Q_LOGGING_CATEGORY(RenderNodes, "Qt3D.Renderer.RHI.RenderNodes", QtWarningMsg)
Q_LOGGING_CATEGORY(Rendering, "Qt3D.Renderer.RHI.Rendering", QtWarningMsg)
Q_LOGGING_CATEGORY(Memory, "Qt3D.Renderer.RHI.Memory", QtWarningMsg)
Q_LOGGING_CATEGORY(Shaders, "Qt3D.Renderer.RHI.Shaders", QtWarningMsg)
Q_LOGGING_CATEGORY(RenderStates, "Qt3D.Renderer.RHI.RenderStates", QtWarningMsg)
Q_LOGGING_CATEGORY(VSyncAdvanceService, "Qt3D.Renderer.RHI.VsyncAdvanceService", QtWarningMsg)

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
