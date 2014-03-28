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

#include "quniformvalue.h"

#include "qgraphicscontext.h"
#include "rendertexture.h"

#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QColor>
#include <QQuaternion>

namespace Qt3D {

namespace Render {

QUniformValue::QUniformValue() :
    m_type(Invalid),
    m_count(-1),
    m_tupleSize(0)
{
}

QUniformValue::QUniformValue(Type type, QVariant value) :
    m_type(type),
    m_count(-1),
    m_tupleSize(0)
{
    fromVariant(value);
    Q_ASSERT((m_tupleSize >= 1) && (m_tupleSize <= 4));
}

QUniformValue::QUniformValue(const QVector4D &f) :
    m_type(Float)
{
    m_var = QVariant(f);
    QVector4D v(f); // need to make a local copy to take the address
    setRawFromFloats(&v[0], 1, 4);
}

void QUniformValue::fromVariant(QVariant v)
{
    if (!v.isValid())
        return;

    if (m_var == v)
        return;
    m_var = v;

    switch (m_type) {
    case Int:
    case UInt:
        fromVariantToInt(v);
        break;

    case Float:
        fromVariantToFloat(v);
        break;

        // Handle doubles if extension supported

    default:
        qWarning() << Q_FUNC_INFO << "unsupported internal type" << m_type;
    }
}

void QUniformValue::fromVariantToInt(QVariant v)
{
    switch (v.type()) {
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Double: {
        qint32 i = static_cast<qint32>(v.toInt());
        setRawFromInts(&i, 1, 1);
        break;
    }

    default:
        qWarning() << Q_FUNC_INFO << "unsupported type" << v;
    }
}

void QUniformValue::fromVariantToFloat(QVariant v)
{
    switch (v.type()) {
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Double: {
        float f = static_cast<float>(v.toDouble());
        setRawFromFloats(&f, 1, 1);
        break;
    }

    case QVariant::Vector2D: {
        QVector2D v2 = v.value<QVector2D>();
        setRawFromFloats(&v2[0], 1, 2);
        break;
    }

    case QVariant::Vector3D: {
        QVector3D v3 = v.value<QVector3D>();
        setRawFromFloats(&v3[0], 1, 3);
        break;
    }

    case QVariant::Vector4D: {
        QVector4D v4 = v.value<QVector4D>();
        setRawFromFloats(&v4[0], 1, 4);
        break;
    }

    case QVariant::Matrix4x4: {
        QMatrix4x4 m = v.value<QMatrix4x4>();
        setRawFromFloats(m.constData(), 4, 4);
        break;
    }

    case QVariant::Color: {
        m_count = 1;
        m_tupleSize = 4;
        m_bytes.resize(sizeof(float) * m_count * m_tupleSize);
        float* bufPtr = reinterpret_cast<float *>(m_bytes.data());
        QColor c = v.value<QColor>();
        *bufPtr++ = c.redF();
        *bufPtr++ = c.greenF();
        *bufPtr++ = c.blueF();
        *bufPtr = c.alphaF();
        break;
    }

    case QVariant::Quaternion: {
        QVector4D qv = v.value<QQuaternion>().toVector4D();
        setRawFromFloats(&qv[0], 1, 4);
        break;
    }

    case QVariant::List: {
        QVariantList vl = v.toList();

        // implement me ...

        break;
    }

    default:
        qWarning() << Q_FUNC_INFO << "unsupported QVariant type";
    }
}

void QUniformValue::setRawFromFloats(const float* ptr, unsigned int count, unsigned int tupleSize)
{
    m_bytes.resize(sizeof(float) * count * tupleSize);
    memcpy(m_bytes.data(), ptr, m_bytes.size());
    m_count = count;
    m_tupleSize = tupleSize;
}

void QUniformValue::setRawFromInts(const qint32* ptr, unsigned int count, unsigned int tupleSize)
{
    m_bytes.resize(sizeof(qint32) * count * tupleSize);
    memcpy(m_bytes.data(), ptr, m_bytes.size());
    m_count = count;
    m_tupleSize = tupleSize;
}

void QUniformValue::apply(QGraphicsContext *gc, int location, QString nm) const
{
    switch (m_type) {
#if 0
    case Int:
        switch (m_tupleSize) {

        }

        gc->activeShader()->setUniformValueArray(location,
                                                 reinterpret_cast<const GLint*>(m_bytes.constData()),
                                                 m_count * m_tupleSize);
                break;

    case UInt:
        gc->activeShader()->setUniformValueArray(location,
                                                 reinterpret_cast<const GLuint*>(m_bytes.constData()),
                                                 m_count * m_tupleSize);
        break;
#endif

    case Float:
        gc->activeShader()->setUniformValueArray(location,
                                                 reinterpret_cast<const GLfloat*>(m_bytes.constData()),
                                                 m_count, m_tupleSize);
        break;

    default:
        qWarning() << Q_FUNC_INFO << "unsupported type:" << m_type;
    }

    int err = glGetError();
    if (err) {
        qWarning() << "error after setting uniform" << m_count << location << m_tupleSize << nm;
    }
}

void QUniformValue::convertToBytes() const
{
    if (!m_bytes.isEmpty())
        return; // use cahced conversion
}

void QUniformPack::setUniform(QString glslName, QUniformValue val)
{
    for (int u=0; u<m_uniforms.size(); ++u) {
        if (m_uniforms[u].glslName != glslName) {
            continue;
        }

        m_uniforms[u].uval = val;
        return;
    }

    NamedUniform nu;
    nu.glslName = glslName;
    nu.uval = val;
    m_uniforms.append(nu);
}

void QUniformPack::setTexture(QString glslName, RenderTexturePtr tex)
{
    for (int t=0; t<m_textures.size(); ++t) {
        if (m_textures[t].glslName != glslName) {
            continue;
        }

        m_textures[t].tex = tex;
        return;
    }

    m_textures.append(NamedTexture(glslName, tex));
}

void QUniformPack::apply(QGraphicsContext *gc)
{
    for (int u=0; u<m_uniforms.size(); ++u) {
        int loc = m_uniforms[u].location;
        if (loc == -1) {
            loc = gc->activeShader()->uniformLocation(m_uniforms[u].glslName);
            m_uniforms[u].location = loc;
        }

        m_uniforms[u].uval.apply(gc, loc, m_uniforms[u].glslName);
    } // of uniforms iteration

    for (int t=0; t<m_textures.size(); ++t) {
        int loc = m_textures[t].location;
        if (loc == -1) {
            loc = gc->activeShader()->uniformLocation(m_textures[t].glslName);
            m_textures[t].location = loc;
        }

        int unit = gc->activateTexture(TextureScopeMaterial, m_textures[t].tex.data());
        gc->activeShader()->setUniformValue(loc, unit);
    } // of textures iteration
}

} // Render

} // of namespace Qt3D

