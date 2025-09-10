// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RHI_SHADERVARIABLES_P_H
#define QT3DRENDER_RENDER_RHI_SHADERVARIABLES_P_H

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
#include <QtGui/private/qshaderdescription_p.h>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

struct ShaderAttribute
{
    QString m_name;
    int m_nameId { -1 };
    QShaderDescription::VariableType m_type {};
    int m_size {};
    int m_location { -1 };
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, Rhi, ShaderAttribute, Q_RELOCATABLE_TYPE)

struct ShaderUniform
{
    QString m_name;
    int m_nameId { -1 };
    QShaderDescription::VariableType m_type { QShaderDescription::Unknown };
    int m_size { 0 };
    int m_offset { -1 }; // -1 default, >= 0 if uniform defined in uniform block
    int m_location { -1 }; // -1 if uniform defined in a uniform block
    int m_blockIndex { -1 }; // -1 is the default, >= 0 if uniform defined in uniform block
    int m_arrayStride {
        -1
    }; // -1 is the default, >= 0 if uniform defined in uniform block and if it's an array
    int m_matrixStride {
        -1
    }; // -1 is the default, >= 0 uniform defined in uniform block and is a matrix
    uint m_rawByteSize { 0 }; // contains byte size (size / type / strides)
    // size, offset and strides are in bytes
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, Rhi, ShaderUniform, Q_RELOCATABLE_TYPE)

struct ShaderUniformBlock
{
    QString m_name;
    int m_nameId { -1 };
    int m_index { -1 };
    int m_binding { -1 };
    int m_activeUniformsCount { 0 };
    int m_size { 0 };
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, Rhi, ShaderUniformBlock, Q_RELOCATABLE_TYPE)

struct ShaderStorageBlock
{
    QString m_name;
    int m_nameId { -1 };
    int m_index { -1 };
    int m_binding { -1 };
    int m_size { 0 };
    int m_activeVariablesCount { 0 };
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, Rhi, ShaderStorageBlock, Q_RELOCATABLE_TYPE)

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_SHADERVARIABLES_P_H
