/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_QUNIFORMVALUE_H
#define QT3DRENDER_RENDER_QUNIFORMVALUE_H

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

#include <QVariant>
#include <QByteArray>
#include <QVector>
#include <QOpenGLShaderProgram>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/shadervariables_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3DCore {
class QFrameAllocator;
}

namespace Qt3DRender {
namespace Render {

class GraphicsContext;

class QUniformValue
{
public:
    enum UniformType {
        Value,
        TextureSampler,
        Unknown
    };

    QUniformValue()
        : m_type(Unknown)
        , m_var()
    {
    }

    explicit QUniformValue(const QVariant &var, UniformType type = Value)
        : m_type(type)
        , m_var(var)
    {
    }

    void setType(UniformType type) Q_DECL_NOTHROW { m_type = type; }
    UniformType type() Q_DECL_NOTHROW { return m_type; }
    bool isTexture() const Q_DECL_NOTHROW { return m_type == TextureSampler; }

    void setValue(const QVariant &value)
    {
        Q_ASSERT(m_type == Value);
        m_var = value;
    }

    QVariant value() const
    {
        Q_ASSERT(m_type == Value);
        return m_var;
    }

    void setTextureUnit(int textureUnit)
    {
        Q_ASSERT(m_type == TextureSampler);
        m_textureIdUnit.m_textureUnit = textureUnit;
    }

    int textureUnit() const
    {
        Q_ASSERT(m_type == TextureSampler);
        return m_textureIdUnit.m_textureUnit;
    }

    void setTextureId(Qt3DCore::QNodeId textureId)
    {
        Q_ASSERT(m_type == TextureSampler);
        m_textureIdUnit.m_textureId = textureId;
    }

    Qt3DCore::QNodeId textureId() const
    {
        Q_ASSERT(m_type == TextureSampler);
        return m_textureIdUnit.m_textureId;
    }

    bool operator ==(const QUniformValue &other)
    {
        if (other.m_type != m_type)
            return false;

        switch (m_type) {
        case Value:
            return other.m_var == m_var;
        case TextureSampler:
            return other.m_textureIdUnit == m_textureIdUnit;
        default:
            break;
        }
        return false;
    }

    bool operator !=(const QUniformValue &other)
    {
        return !operator ==(other);
    }

    void apply(GraphicsContext *ctx, const ShaderUniform &description) const;

protected:
    struct TextureIdUnit {
        Qt3DCore::QNodeId m_textureId;
        int m_textureUnit;

        TextureIdUnit()
            : m_textureId()
            , m_textureUnit(-1)
        {}

        bool operator == (const TextureIdUnit &other) const Q_DECL_NOTHROW
        {
            return (other.m_textureId == m_textureId) && (other.m_textureUnit == m_textureUnit);
        }

        bool operator !=(const TextureIdUnit &other) const Q_DECL_NOTHROW
        {
            return !operator ==(other);
        }
    };

    // TODO: Replace QVariant with our own union of GLSL types as we don't
    // need the full flexibility of QVariant on the backend
    UniformType m_type;
    QVariant m_var;
    TextureIdUnit m_textureIdUnit;
};

struct BlockToUBO {
    int m_blockIndex;
    Qt3DCore::QNodeId m_bufferID;
    bool m_needsUpdate;
    QHash<QString, QVariant> m_updatedProperties;
};
QT3D_DECLARE_TYPEINFO_2(Qt3DRender, Render, BlockToUBO, Q_MOVABLE_TYPE)

struct BlockToSSBO {
    int m_blockIndex;
    Qt3DCore::QNodeId m_bufferID;
};
QT3D_DECLARE_TYPEINFO_2(Qt3DRender, Render, BlockToSSBO, Q_PRIMITIVE_TYPE)


typedef QHash<int, QUniformValue> PackUniformHash;

class ShaderParameterPack
{
public:
    ~ShaderParameterPack();

    void setUniform(const int glslNameId, const QUniformValue &val);
    void setTexture(const int glslNameId, Qt3DCore::QNodeId id);
    void setUniformBuffer(BlockToUBO blockToUBO);
    void setShaderStorageBuffer(BlockToSSBO blockToSSBO);

    inline PackUniformHash &uniforms() { return m_uniforms; }
    inline const PackUniformHash &uniforms() const { return m_uniforms; }
    QUniformValue uniform(const int glslNameId) const { return m_uniforms.value(glslNameId); }

    struct NamedTexture
    {
        NamedTexture() {}
        NamedTexture(const int nm, Qt3DCore::QNodeId t)
            : glslNameId(nm)
            , texId(t)
        { }

        int glslNameId;
        Qt3DCore::QNodeId texId;
    };

    inline QVector<NamedTexture> textures() const { return m_textures; }
    inline QVector<BlockToUBO> uniformBuffers() const { return m_uniformBuffers; }
    inline QVector<BlockToSSBO> shaderStorageBuffers() const { return m_shaderStorageBuffers; }
private:
    PackUniformHash m_uniforms;

    QVector<NamedTexture> m_textures;
    QVector<BlockToUBO> m_uniformBuffers;
    QVector<BlockToSSBO> m_shaderStorageBuffers;

    friend class RenderView;
};
QT3D_DECLARE_TYPEINFO_2(Qt3DRender, Render, ShaderParameterPack::NamedTexture, Q_PRIMITIVE_TYPE)

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUNIFORMVALUE_H
