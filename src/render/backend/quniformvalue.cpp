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
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

QUniformValue::QUniformValue() :
    m_type(Invalid),
    m_count(-1),
    m_tupleSize(0)
{
}

QUniformValue *QUniformValue::fromVariant(const QVariant &v)
{
    switch (v.type()) {
    case QVariant::Int: return new SpecifiedUniform<int>(v.toInt());
    case QVariant::UInt: return new SpecifiedUniform<uint>(v.toUInt());
    case QVariant::Double: return new SpecifiedUniform<float>(v.toFloat());
    case QVariant::Vector2D: return new SpecifiedUniform<QVector2D>(v.value<QVector2D>());
    case QVariant::Vector3D: return new SpecifiedUniform<QVector3D>(v.value<QVector3D>());
    case QVariant::Vector4D: return new SpecifiedUniform<QVector4D>(v.value<QVector4D>());
    case QVariant::Matrix4x4: return new SpecifiedUniform<QMatrix4x4>(v.value<QMatrix4x4>());
    case QVariant::Point: return new SpecifiedUniform<QPoint>(v.value<QPoint>());
    case QVariant::PointF: return new SpecifiedUniform<QPointF>(v.value<QPointF>());
    case QVariant::Size: return new SpecifiedUniform<QSize>(v.value<QSize>());
    case QVariant::SizeF: return new SpecifiedUniform<QSizeF>(v.value<QSizeF>());
    case QVariant::Transform: return new SpecifiedUniform<QTransform>(v.value<QTransform>());
    case QVariant::Color: return new SpecifiedUniform<QColor>(v.value<QColor>());
    case QVariant::Quaternion: return new SpecifiedUniform<QVector4D>(v.value<QVector4D>());
    default: return new QUniformValue();
    }
}

bool QUniformValue::operator ==(const QUniformValue &other)
{
    return (other.m_type == this->m_type &&
            other.m_tupleSize == this->m_tupleSize &&
            other.m_count == this->m_count &&
            other.m_var == this->m_var);
}

bool QUniformValue::operator !=(const QUniformValue &other)
{
    return !operator ==(other);
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

void QUniformValue::apply(QOpenGLShaderProgram *prog, int location, const QString &name) const
{
    switch (m_type) {
#if 0
    case Int:
        switch (m_tupleSize) {

        }

        prog->setUniformValueArray(location,
                                   reinterpret_cast<const GLint*>(m_bytes.constData()),
                                   m_count * m_tupleSize);
        break;

    case UInt:
        prog->setUniformValueArray(location,
                                   reinterpret_cast<const GLuint*>(m_bytes.constData()),
                                   m_count * m_tupleSize);
        break;
#endif

    case Float:
        prog->setUniformValueArray(location,
                                   reinterpret_cast<const GLfloat*>(m_bytes.constData()),
                                   m_count, m_tupleSize);
        break;

    default:
        qWarning() << Q_FUNC_INFO << "unsupported type:" << m_type << "for " << name;
    }

    int err = glGetError();
    if (err) {
//        qCWarning(Render::Backend) << Q_FUNC_INFO << "error " << err << "after setting uniform" << m_count << location << m_tupleSize << name;
    }
}

void QUniformValue::convertToBytes() const
{
    if (!m_bytes.isEmpty())
        return; // use cahced conversion
}

QUniformPack::~QUniformPack()
{
    // TO DO : Should be a qDeleteAll but crashes for some reason at the moment
    // Will be fixed later as this is going to evolve

    m_uniforms.clear();
}

void QUniformPack::setUniform(QString glslName, const QUniformValue *val)
{
    m_uniforms[glslName] = val;
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

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
