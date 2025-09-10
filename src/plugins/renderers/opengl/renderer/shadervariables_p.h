// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_OPENGL_SHADERVARIABLES_P_H
#define QT3DRENDER_RENDER_OPENGL_SHADERVARIABLES_P_H

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

#include <Qt3DRender/qt3drender_global.h>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

struct ShaderAttribute
{
    ShaderAttribute()
        : m_nameId(-1)
        , m_type(0)
        , m_size(0)
        , m_location(-1)
    {}

    QString m_name;
    int m_nameId;
    GLenum m_type;
    int m_size;
    int m_location;
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, OpenGL, ShaderAttribute, Q_RELOCATABLE_TYPE)

struct Q_AUTOTEST_EXPORT ShaderUniform
{
    ShaderUniform()
        : m_nameId(-1)
        , m_type(GL_NONE)
        , m_size(0)
        , m_offset(-1)
        , m_location(-1)
        , m_blockIndex(-1)
        , m_arrayStride(-1)
        , m_matrixStride(-1)
        , m_rawByteSize(0)
    {}

    QString m_name;
    int m_nameId;
    GLenum m_type;
    int m_size;
    int m_offset; // -1 default, >= 0 if uniform defined in uniform block
    int m_location; // -1 if uniform defined in a uniform block
    int m_blockIndex; // -1 is the default, >= 0 if uniform defined in uniform block
    int m_arrayStride; // -1 is the default, >= 0 if uniform defined in uniform block and if it's an array
    int m_matrixStride; // -1 is the default, >= 0 uniform defined in uniform block and is a matrix
    uint m_rawByteSize; // contains byte size (size / type / strides)
    // size, offset and strides are in bytes
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, OpenGL, ShaderUniform, Q_RELOCATABLE_TYPE)

struct Q_AUTOTEST_EXPORT ShaderUniformBlock
{
    ShaderUniformBlock()
        : m_nameId(-1)
        , m_index(-1)
        , m_binding(-1)
        , m_activeUniformsCount(0)
        , m_size(0)
    {}

    QString m_name;
    int m_nameId;
    int m_index;
    int m_binding;
    int m_activeUniformsCount;
    int m_size;
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, OpenGL, ShaderUniformBlock, Q_RELOCATABLE_TYPE)

struct Q_AUTOTEST_EXPORT ShaderStorageBlock
{
    ShaderStorageBlock()
        : m_nameId(-1)
        , m_index(-1)
        , m_binding(-1)
        , m_size(0)
        , m_activeVariablesCount(0)
    {}

    QString m_name;
    int m_nameId;
    int m_index;
    int m_binding;
    int m_size;
    int m_activeVariablesCount;
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, OpenGL, ShaderStorageBlock, Q_RELOCATABLE_TYPE)

} // namespace OpenGL

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_SHADERVARIABLES_P_H
