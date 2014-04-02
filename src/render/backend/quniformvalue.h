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

// for RenderTexturePtr
#include <rendertextureprovider.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
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
        Bool
    };

    QUniformValue();

    QUniformValue(Type type, QVariant value);

    // explicit construction from various types without going through
    // QVariant boxing + unboxing. Add as needed if this proves useful
    explicit QUniformValue(const QVector4D& foo);

    void apply(QGraphicsContext* gc, int location, QString nm) const;

private:
    void convertToBytes() const;

    Type m_type;
    int m_count;
    unsigned int m_tupleSize;

    QVariant m_var;
    // value as data suitable for passing to glUniformfv/iv
    mutable QByteArray m_bytes;

    void fromVariant(QVariant v);
    void fromVariantToInt(QVariant v);
    void fromVariantToFloat(QVariant v);

    void setRawFromFloats(const float *ptr, unsigned int count, unsigned int tupleSize);
    void setRawFromInts(const qint32 *ptr, unsigned int count, unsigned int tupleSize);
};

class QUniformPack
{
public:
    void setUniform(QString glslName, QUniformValue val);

    void setTexture(QString glslName, RenderTexturePtr tex);

    void apply(QGraphicsContext* gc);
private:

    struct NamedUniform {
        NamedUniform() : location(-1) { }

        QString glslName;
        int location;
        QUniformValue uval;
    };

    QVector<NamedUniform> m_uniforms;

    struct NamedTexture {
        NamedTexture() : location(-1) {}

        NamedTexture(QString nm, RenderTexturePtr t) :
            glslName(nm),
            location(-1),
            tex(t)
        { }

        QString glslName;
        int location;
        RenderTexturePtr tex;
    };

    QVector<NamedTexture> m_textures;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QUNIFORMVALUE_H
