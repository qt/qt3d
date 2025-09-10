// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "renderlogging_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

Q_LOGGING_CATEGORY(Backend, "Qt3D.Renderer.Backend", QtWarningMsg)
Q_LOGGING_CATEGORY(Frontend, "Qt3D.Renderer.Frontend", QtWarningMsg)
Q_LOGGING_CATEGORY(Io, "Qt3D.Renderer.IO", QtWarningMsg)
Q_LOGGING_CATEGORY(Jobs, "Qt3D.Renderer.Jobs", QtWarningMsg)
Q_LOGGING_CATEGORY(SceneLoaders, "Qt3D.Renderer.SceneLoaders", QtWarningMsg)
Q_LOGGING_CATEGORY(Framegraph, "Qt3D.Renderer.Framegraph", QtWarningMsg)
Q_LOGGING_CATEGORY(RenderNodes, "Qt3D.Renderer.RenderNodes", QtWarningMsg)
Q_LOGGING_CATEGORY(Rendering, "Qt3D.Renderer.Rendering", QtWarningMsg)
Q_LOGGING_CATEGORY(Memory, "Qt3D.Renderer.Memory", QtWarningMsg)
Q_LOGGING_CATEGORY(Shaders, "Qt3D.Renderer.Shaders", QtWarningMsg)
Q_LOGGING_CATEGORY(RenderStates, "Qt3D.Renderer.RenderStates", QtWarningMsg)
Q_LOGGING_CATEGORY(VSyncAdvanceService, "Qt3D.Renderer.VsyncAdvanceService", QtWarningMsg)
Q_LOGGING_CATEGORY(ShaderCache, "Qt3D.Renderer.ShaderCache", QtWarningMsg)

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
