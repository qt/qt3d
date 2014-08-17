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
#include <QUuid>
#include <Qt3DRenderer/renderlogging.h>

// for RenderTexturePtr
#include <Qt3DRenderer/private/rendertextureprovider_p.h>

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
    enum Type {
        Invalid = 0,
        Int = 1,
        UInt = 2,
        Float = 3,
        Double = 4,
        Bool,
        Texture,
        Custom
    };

    QUniformValue();
    virtual ~QUniformValue() {}

    static QUniformValue *fromVariant(const QVariant &v, QFrameAllocator *allocator);

    virtual bool operator ==(const QUniformValue &other);
    bool operator !=(const QUniformValue &other);

    virtual bool isValid() const { return m_type != Invalid; }

    virtual void apply(QOpenGLShaderProgram* prog, int location, const QString &name) const;

    // For advanced use cases not handled by fromVariant
    void setRawFromFloats(const float *ptr, unsigned int count, unsigned int tupleSize);
    void setRawFromInts(const qint32 *ptr, unsigned int count, unsigned int tupleSize);

protected:
    Type m_type;
    QVariant m_var;

private:
    void convertToBytes() const;
    int m_count;
    unsigned int m_tupleSize;

    // value as data suitable for passing to glUniformfv/iv
    mutable QByteArray m_bytes;

};


template <typename T>
class SpecifiedUniform : public QUniformValue
{
public :
    SpecifiedUniform()
        : QUniformValue()
    {
        m_type = Custom;
    }

    void setValue(const T &value) { m_value = value; }

    bool operator ==(const QUniformValue &other) Q_DECL_OVERRIDE
    {
        Q_UNUSED(other);
        // TO DO : find a way to improve that
        return false;
    }

    bool isValid() const Q_DECL_OVERRIDE { return true; }

    void apply(QOpenGLShaderProgram *prog, int location, const QString &name) const Q_DECL_OVERRIDE
    {
        prog->setUniformValue(location, (T)m_value);
        int err = glGetError();
        if (err) {
            qCWarning(Render::Backend, "Error %d after setting uniform \"%s\" at location %d",
                      err, qUtf8Printable(name), location);
        }
    }

private :
    T m_value;
};

class TextureUniform : public QUniformValue
{
public :
    TextureUniform()
        : QUniformValue()
        , m_textureUnit(-1)
    {
        m_type = Texture;
    }

    void setTextureId(const QUuid &id)
    {
        m_textureId = id;
    }

    QUuid textureId() const { return m_textureId; }

    bool operator ==(const QUniformValue &other) Q_DECL_OVERRIDE
    {
        Q_UNUSED(other);
        return false;
    }

    // Called by the QGraphicContext prior applying
    void setTextureUnit(int textureUnit) { m_textureUnit = textureUnit; }

    bool isValid() const Q_DECL_OVERRIDE { return true; }

    void apply(QOpenGLShaderProgram *prog, int location, const QString &name) const
    {
        // We assume that the texture has been successfully bound and attache to a texture unit
        if (m_textureUnit != -1) {
            prog->setUniformValue(location, m_textureUnit);
            int err = glGetError();
            if (err) {
                qCWarning(Render::Backend, "Error %d after setting uniform \"%s\" at location %d",
                          err, qUtf8Printable(name), location);
            }
        } else {
            qCWarning(Render::Backend, "Invalid texture unit supplied for \"%s\"",
                      qUtf8Printable(name));
        }
    }

private:
    QUuid m_textureId;
    int m_textureUnit;
};

class QUniformPack
{
public:
    ~QUniformPack();

    void setUniform(QString glslName, const QUniformValue *val);
    void setTexture(QString glslName, const QUuid &id);

    const QHash<QString, const QUniformValue* > &uniforms() const { return m_uniforms; }

    struct NamedTexture {
        NamedTexture() {}
        NamedTexture(QString nm, const QUuid &t) :
            glslName(nm),
            texId(t)
        { }

        QString glslName;
        QUuid texId;
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
