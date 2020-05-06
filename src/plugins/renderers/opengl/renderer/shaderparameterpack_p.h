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

#ifndef QT3DRENDER_RENDER_OPENGL_SHADERPARAMETERPACK_P_H
#define QT3DRENDER_RENDER_OPENGL_SHADERPARAMETERPACK_P_H

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
#include <Qt3DRender/private/uniform_p.h>
#include <shadervariables_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3DCore {
class QFrameAllocator;
}

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

class GraphicsContext;

struct BlockToUBO {
    int m_blockIndex;
    Qt3DCore::QNodeId m_bufferID;
    bool m_needsUpdate;
    QHash<QString, QVariant> m_updatedProperties;
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, OpenGL, BlockToUBO, Q_MOVABLE_TYPE)

struct BlockToSSBO {
    int m_blockIndex;
    int m_bindingIndex;
    Qt3DCore::QNodeId m_bufferID;
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, OpenGL, BlockToSSBO, Q_PRIMITIVE_TYPE)


struct PackUniformHash
{
    std::vector<int> keys;
    std::vector<UniformValue> values;

    PackUniformHash()
    {
    }

    void reserve(int count)
    {
        keys.reserve(count);
        values.reserve(count);
    }

    inline int indexForKey(int key) const
    {
        const auto b = keys.cbegin();
        const auto e = keys.cend();
        const auto it = std::find(b, e, key);
        if (it == e)
            return -1;
        return std::distance(b, it);
    }

    void insert(int key, const UniformValue &value)
    {
        const int idx = indexForKey(key);
        if (idx != -1) {
            values[idx] = value;
        } else {
            keys.push_back(key);
            values.push_back(value);
        }
    }

    void insert(int key, UniformValue &&value)
    {
        const int idx = indexForKey(key);
        if (idx != -1) {
            values[idx] = std::move(value);
        } else {
            keys.push_back(key);
            values.push_back(std::move(value));
        }
    }

    UniformValue value(int key) const noexcept
    {
        const int idx = indexForKey(key);
        if (idx != -1)
            return values.at(idx);
        return UniformValue();
    }

    UniformValue& value(int key)
    {
        const int idx = indexForKey(key);
        if (idx != -1)
            return values[idx];
        insert(key, UniformValue());
        return value(key);
    }

    template<typename F>
    void apply(int key, F func) const noexcept
    {
        const int idx = indexForKey(key);
        if (idx != -1)
            func(values[idx]);
    }

    void erase(int idx)
    {
        keys.erase(keys.begin() + idx);
        values.erase(values.begin() + idx);
    }

    bool contains(int key) const noexcept
    {
        const auto b = keys.cbegin();
        const auto e = keys.cend();
        return std::find(b, e, key) != e;
    }
};

class Q_AUTOTEST_EXPORT ShaderParameterPack
{
public:
    ~ShaderParameterPack();

    void reserve(int uniformCount);
    void setUniform(const int glslNameId, const UniformValue &val);
    void setTexture(const int glslNameId, int uniformArrayIndex, Qt3DCore::QNodeId id);
    void setImage(const int glslNameId, int uniformArrayIndex, Qt3DCore::QNodeId id);

    void setUniformBuffer(BlockToUBO blockToUBO);
    void setShaderStorageBuffer(BlockToSSBO blockToSSBO);
    void setSubmissionUniformIndex(const int shaderUniformIndex);

    inline PackUniformHash &uniforms() { return m_uniforms; }
    inline const PackUniformHash &uniforms() const { return m_uniforms; }
    UniformValue uniform(const int glslNameId) const { return m_uniforms.value(glslNameId); }


    struct NamedResource
    {
        enum Type {
            Texture = 0,
            Image
        };

        NamedResource() {}
        NamedResource(const int glslNameId, Qt3DCore::QNodeId texId,
                      int uniformArrayIndex, Type type)
            : glslNameId(glslNameId)
            , nodeId(texId)
            , uniformArrayIndex(uniformArrayIndex)
            , type(type)
        { }

        int glslNameId;
        Qt3DCore::QNodeId nodeId;
        int uniformArrayIndex;
        Type type;

        bool operator==(const NamedResource &other) const
        {
            return glslNameId == other.glslNameId &&
                    nodeId == other.nodeId &&
                    uniformArrayIndex == other.uniformArrayIndex &&
                    type == other.type;
        }

        bool operator!=(const NamedResource &other) const
        {
            return !(*this == other);
        }
    };

    inline const std::vector<NamedResource> &textures() const { return m_textures; }
    inline const std::vector<NamedResource> &images() const { return m_images; }
    inline const std::vector<BlockToUBO> &uniformBuffers() const { return m_uniformBuffers; }
    inline const std::vector<BlockToSSBO> &shaderStorageBuffers() const { return m_shaderStorageBuffers; }
    inline const std::vector<int> &submissionUniformIndices() const { return m_submissionUniformIndices; }
private:
    PackUniformHash m_uniforms;

    std::vector<NamedResource> m_textures;
    std::vector<NamedResource> m_images;
    std::vector<BlockToUBO> m_uniformBuffers;
    std::vector<BlockToSSBO> m_shaderStorageBuffers;
    std::vector<int> m_submissionUniformIndices;

    friend class RenderView;
};
QT3D_DECLARE_TYPEINFO_3(Qt3DRender, Render, OpenGL, ShaderParameterPack::NamedResource, Q_PRIMITIVE_TYPE)

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Render::OpenGL::ShaderParameterPack)

#endif // QT3DRENDER_RENDER_OPENGL_SHADERPARAMETERPACK_P_H
