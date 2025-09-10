// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_HANDLE_TYPES_P_H
#define QT3DRENDER_RENDER_OPENGL_HANDLE_TYPES_P_H

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

namespace OpenGL {

class GLBuffer;
class GLTexture;
class OpenGLVertexArrayObject;

typedef Qt3DCore::QHandle<GLBuffer> HGLBuffer;
typedef Qt3DCore::QHandle<OpenGLVertexArrayObject> HVao;
typedef Qt3DCore::QHandle<GLTexture> HGLTexture;

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_HANDLE_TYPES_P_H
