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

#include <Qt3DCore/qframeallocator.h>

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

QUniformValue *QUniformValue::fromVariant(const QVariant &v, QFrameAllocator *allocator)
{
    switch (v.type()) {
    case QVariant::Int: {
        SpecifiedUniform<int> *t = allocator->allocate<SpecifiedUniform<int> >();
        t->setValue(v.toInt());
        return t;
    }
    case QVariant::UInt: {
        SpecifiedUniform<uint> *t = allocator->allocate<SpecifiedUniform<uint> >();
        t->setValue(v.toUInt());
        return t;
    }
    case QVariant::Double: {
        SpecifiedUniform<float> *t = allocator->allocate<SpecifiedUniform<float> >();
        t->setValue(v.toFloat());
        return t;
    }
    case QVariant::Vector2D: {
        SpecifiedUniform<QVector2D> *t = allocator->allocate<SpecifiedUniform<QVector2D> >();
        t->setValue(v.value<QVector2D>());
        return t;
    }
    case QVariant::Vector3D: {
        SpecifiedUniform<QVector3D> *t = allocator->allocate<SpecifiedUniform<QVector3D> >();
        t->setValue(v.value<QVector3D>());
        return t;
    }
    case QVariant::Vector4D: {
        SpecifiedUniform<QVector4D> *t = allocator->allocate<SpecifiedUniform<QVector4D> >();
        t->setValue(v.value<QVector4D>());
        return t;
    }
    case QVariant::Matrix4x4: {
        SpecifiedUniform<QMatrix4x4> *t = allocator->allocate<SpecifiedUniform<QMatrix4x4> >();
        t->setValue(v.value<QMatrix4x4>());
        return t;
    }
    case QVariant::Point: {
        SpecifiedUniform<QPoint> *t = allocator->allocate<SpecifiedUniform<QPoint> >();
        t->setValue(v.value<QPoint>());
        return t;
    }
    case QVariant::PointF: {
        SpecifiedUniform<QPointF> *t = allocator->allocate<SpecifiedUniform<QPointF> >();
        t->setValue(v.value<QPointF>());
        return t;
    }
    case QVariant::Size: {
        SpecifiedUniform<QSize> *t = allocator->allocate<SpecifiedUniform<QSize> >();
        t->setValue(v.value<QSize>());
        return t;
    }
    case QVariant::SizeF: {
        SpecifiedUniform<QSizeF> *t = allocator->allocate<SpecifiedUniform<QSizeF> >();
        t->setValue(v.value<QSizeF>());
        return t;
    }
    case QVariant::Transform: {
        SpecifiedUniform<QTransform> *t = allocator->allocate<SpecifiedUniform<QTransform> >();
        t->setValue(v.value<QTransform>());
        return t;
    }
    case QVariant::Color: {
        SpecifiedUniform<QColor> *t = allocator->allocate<SpecifiedUniform<QColor> >();
        t->setValue(v.value<QColor>());
        return t;
    }
    case QVariant::Quaternion: {
        SpecifiedUniform<QVector4D> *t = allocator->allocate<SpecifiedUniform<QVector4D> >();
        t->setValue(v.value<QVector4D>());
        return t;
    }
    default:
        return allocator->allocate<QUniformValue>();
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
        qCWarning(Render::Backend) << Q_FUNC_INFO << "error " << err << "after setting uniform" << m_count << location << m_tupleSize << name;
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

void QUniformPack::setTexture(QString glslName, const QUuid &texId)
{
    for (int t=0; t<m_textures.size(); ++t) {
        if (m_textures[t].glslName != glslName) {
            continue;
        }

        m_textures[t].texId = texId;
        return;
    }

    m_textures.append(NamedTexture(glslName, texId));
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
