// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef GLFENCE_P_H
#define GLFENCE_P_H

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

#include <QtGlobal>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

// GLsync is a pointer to a struct (unlike the rest of GL which used int ids)
// We cannot reference GLsync as it's only available since 3.2 We use FenceId
// to wrap that around and trust the GLHelpers will convert them accordingly.
using GLFence = void *;

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // GLFENCE_P_H
