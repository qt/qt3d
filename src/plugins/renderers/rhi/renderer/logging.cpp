/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "logging_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

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

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
