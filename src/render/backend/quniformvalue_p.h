/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_QUNIFORMVALUE_H
#define QT3D_RENDER_QUNIFORMVALUE_H

#include <QVariant>
#include <QByteArray>
#include <QVector>
#include <QOpenGLShaderProgram>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRenderer/private/renderlogging_p.h>

// for RenderTexturePtr
#include <Qt3DRenderer/private/rendertextureprovider_p.h>
#include <Qt3DRenderer/private/shadervariables_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLShaderProgram;

namespace Qt3D {

class QFrameAllocator;

namespace Render {

class QGraphicsContext;
/**
 * @brief The QUniformValue class - immutable storage of uniform value
 * in the rendering backend.
 */
class QUniformValue
{
public:
    QUniformValue();
    virtual ~QUniformValue() {}

    static QUniformValue *fromVariant(const QVariant &v, QFrameAllocator *allocator);

    virtual bool operator ==(const QUniformValue &other);
    bool operator !=(const QUniformValue &other);

    virtual void apply(QGraphicsContext *ctx, const ShaderUniform &description) const;

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

    void setTextureId(const QNodeId &id)
    {
        m_textureId = id;
    }

    QNodeId textureId() const { return m_textureId; }

    bool operator ==(const QUniformValue &other) Q_DECL_OVERRIDE
    {
        Q_UNUSED(other);
        return false;
    }

    // Called by the QGraphicContext prior applying
    void setTextureUnit(int textureUnit) { m_textureUnit = textureUnit; }

    void apply(QGraphicsContext *ctx, const ShaderUniform &description) const;

private:
    QNodeId m_textureId;
    int m_textureUnit;
};

class QUniformPack
{
public:
    ~QUniformPack();

    void setUniform(const QString &glslName, const QUniformValue *val);
    void setTexture(const QString &glslName, const QNodeId &id);

    const QHash<QString, const QUniformValue* > &uniforms() const { return m_uniforms; }
    QHash<QString, const QUniformValue* > &uniforms() { return m_uniforms; }

    struct NamedTexture
    {
        NamedTexture() {}
        NamedTexture(const QString &nm, const QNodeId &t)
            : glslName(nm)
            , texId(t)
        { }

        QString glslName;
        QNodeId texId;
    };

    QVector<NamedTexture> textures() const { return m_textures; }

private:
    QHash<QString, const QUniformValue* > m_uniforms;

    QVector<NamedTexture> m_textures;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QUNIFORMVALUE_H
