// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "logging_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

Q_LOGGING_CATEGORY(Backend, "Qt3D.Renderer.OpenGL.Backend", QtWarningMsg)
Q_LOGGING_CATEGORY(Frontend, "Qt3D.Renderer.OpenGL.Frontend", QtWarningMsg)
Q_LOGGING_CATEGORY(Io, "Qt3D.Renderer.OpenGL.IO", QtWarningMsg)
Q_LOGGING_CATEGORY(Jobs, "Qt3D.Renderer.OpenGL.Jobs", QtWarningMsg)
Q_LOGGING_CATEGORY(SceneLoaders, "Qt3D.Renderer.OpenGL.SceneLoaders", QtWarningMsg)
Q_LOGGING_CATEGORY(Framegraph, "Qt3D.Renderer.OpenGL.Framegraph", QtWarningMsg)
Q_LOGGING_CATEGORY(RenderNodes, "Qt3D.Renderer.OpenGL.RenderNodes", QtWarningMsg)
Q_LOGGING_CATEGORY(Rendering, "Qt3D.Renderer.OpenGL.Rendering", QtWarningMsg)
Q_LOGGING_CATEGORY(Memory, "Qt3D.Renderer.OpenGL.Memory", QtWarningMsg)
Q_LOGGING_CATEGORY(Shaders, "Qt3D.Renderer.OpenGL.Shaders", QtWarningMsg)
Q_LOGGING_CATEGORY(RenderStates, "Qt3D.Renderer.OpenGL.RenderStates", QtWarningMsg)
Q_LOGGING_CATEGORY(VSyncAdvanceService, "Qt3D.Renderer.OpenGL.VsyncAdvanceService", QtWarningMsg)

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
