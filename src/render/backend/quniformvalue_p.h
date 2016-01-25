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
/**
 * @brief The QUniformValue class - immutable storage of uniform value
 * in the rendering backend.
 */
class QUniformValue
{
public:
    QUniformValue();
    virtual ~QUniformValue() {}

    static QUniformValue *fromVariant(const QVariant &v, Qt3DCore::QFrameAllocator *allocator);

    virtual bool operator ==(const QUniformValue &other);
    bool operator !=(const QUniformValue &other);

    virtual bool isTexture() const
    {
        return false;
    }

    virtual void apply(GraphicsContext *ctx, const ShaderUniform &description) const;

protected:
    QVariant m_var;
};

class TextureUniform : public QUniformValue
{
public :
    TextureUniform()
        : QUniformValue()
        , m_textureUnit(-1)
    {
    }

    bool isTexture() const Q_DECL_FINAL
    {
        return true;
    }

    void setTextureId(const Qt3DCore::QNodeId &id)
    {
        m_textureId = id;
    }

    Qt3DCore::QNodeId textureId() const { return m_textureId; }

    bool operator ==(const QUniformValue &other) Q_DECL_OVERRIDE
    {
        if (other.isTexture()) {
            const TextureUniform *otherTex = static_cast<const TextureUniform *>(&other);
            return otherTex->textureId() == m_textureId;
        }
        return false;
    }

    // Called by the QGraphicContext prior applying
    void setTextureUnit(int textureUnit) { m_textureUnit = textureUnit; }

    void apply(GraphicsContext *ctx, const ShaderUniform &description) const Q_DECL_OVERRIDE;

private:
    Qt3DCore::QNodeId m_textureId;
    int m_textureUnit;
};

struct BlockToUBO {
    int m_blockIndex;
    Qt3DCore::QNodeId m_bufferID;
    bool m_needsUpdate;
    QHash<QString, QVariant> m_updatedProperties;
};

struct BlockToSSBO {
    int m_blockIndex;
    Qt3DCore::QNodeId m_bufferID;
};

class ShaderParameterPack
{
public:
    ~ShaderParameterPack();

    void setUniform(const QString &glslName, const QUniformValue *val);
    void setTexture(const QString &glslName, const Qt3DCore::QNodeId &id);
    void setUniformBuffer(const BlockToUBO &blockToUBO);
    void setShaderStorageBuffer(const BlockToSSBO &blockToSSBO);

    inline const QHash<QString, const QUniformValue* > &uniforms() const { return m_uniforms; }
    const QUniformValue *uniform(const QString &glslName) const { return m_uniforms.value(glslName); }

    struct NamedTexture
    {
        NamedTexture() {}
        NamedTexture(const QString &nm, const Qt3DCore::QNodeId &t)
            : glslName(nm)
            , texId(t)
        { }

        QString glslName;
        Qt3DCore::QNodeId texId;
    };

    inline QVector<NamedTexture> textures() const { return m_textures; }
    inline QVector<BlockToUBO> uniformBuffers() const { return m_uniformBuffers; }
    inline QVector<BlockToSSBO> shaderStorageBuffers() const { return m_shaderStorageBuffers; }
private:
    QHash<QString, const QUniformValue* > m_uniforms;

    QVector<NamedTexture> m_textures;
    QVector<BlockToUBO> m_uniformBuffers;
    QVector<BlockToSSBO> m_shaderStorageBuffers;

    friend class RenderView;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUNIFORMVALUE_H
