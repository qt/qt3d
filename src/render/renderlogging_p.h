// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_RENDER_RENDERLOGGING_P_H
#define QT3DRENDER_RENDER_RENDERLOGGING_P_H

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

#include <QLoggingCategory>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

Q_DECLARE_LOGGING_CATEGORY(Backend)
Q_DECLARE_LOGGING_CATEGORY(Frontend)
Q_DECLARE_LOGGING_CATEGORY(Io)
Q_DECLARE_LOGGING_CATEGORY(Jobs)
Q_DECLARE_LOGGING_CATEGORY(SceneLoaders)
Q_DECLARE_LOGGING_CATEGORY(Framegraph)
Q_DECLARE_LOGGING_CATEGORY(RenderNodes)
Q_DECLARE_LOGGING_CATEGORY(Rendering)
Q_DECLARE_LOGGING_CATEGORY(Memory)
Q_DECLARE_LOGGING_CATEGORY(Shaders)
Q_DECLARE_LOGGING_CATEGORY(RenderStates)
Q_DECLARE_LOGGING_CATEGORY(VSyncAdvanceService)
Q_DECLARE_LOGGING_CATEGORY(ShaderCache)

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERLOGGING_P_H
