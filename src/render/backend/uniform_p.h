/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_UNIFORM_P_H
#define QT3DRENDER_RENDER_UNIFORM_P_H

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

#include <qt3drender_global.h>
#include <Qt3DCore/qnodeid.h>

#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QColor>

#include <QDebug>
#include <string.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

enum UniformType {
    Float = 0,
    Vec2,
    Vec3,
    Vec4,
    Double,
    DVec2,
    DVec3,
    DVec4,
    Int,
    IVec2,
    IVec3,
    IVec4,
    UInt,
    UIVec2,
    UIVec3,
    UIVec4,
    Bool,
    BVec2,
    BVec3,
    BVec4,
    Mat2,
    Mat3,
    Mat4,
    Mat2x3,
    Mat3x2,
    Mat2x4,
    Mat4x2,
    Mat3x4,
    Mat4x3,
    Sampler,
    Unknown
};

class Q_AUTOTEST_EXPORT UniformValue
{
public:
    enum ValueType {
        ScalarValue,
        NodeId,
        TextureValue,
        BufferValue
    };

    struct Texture {
        int textureId = 0; // Set first so that glUniform1iv will work
        Qt3DCore::QNodeId nodeId;
    };

    // UniformValue implicitely converts doubles to floats to ensure
    // correct rendering behavior for the cases where Qt3D parameters created from
    // a double or QVariant(double) are used to fill uniform values that in reality
    // should be floats. This occur especially with QML where qreal might be double
    // and not float. Otherwise, at when filling the uniforms, calling constData<float>
    // on something that contains a double will result in wrong values

    UniformValue() {}
    UniformValue(int i) { m_data.ivec[0] = i; }
    UniformValue(uint i) { m_data.ivec[0] = i; }
    UniformValue(float f) { m_data.fvec[0] = f; }
    UniformValue(double d) { m_data.fvec[0] = d; } // Double to float conversion
    UniformValue(bool b) { m_data.ivec[0] = b; }
    UniformValue(const QVector2D &vec2) { memcpy(&m_data, &vec2, sizeof(QVector2D)); }
    UniformValue(const QVector3D &vec3) { memcpy(&m_data, &vec3, sizeof(QVector3D)); }
    UniformValue(const QVector4D &vec4) { memcpy(&m_data, &vec4, sizeof(QVector4D)); }

    UniformValue(const QMatrix3x3 &mat33)
    {
        // Use constData because we want column-major layout
        memcpy(&m_data, mat33.constData(), 9 * sizeof(float));
    }

    UniformValue(const QMatrix4x4 &mat44)
    {
        // Use constData because we want column-major layout
        memcpy(&m_data, mat44.constData(), 16 * sizeof(float));
    }

    // For nodes which will later be replaced by a Texture or Buffer
    UniformValue(Qt3DCore::QNodeId id)
    {
        m_valueType = NodeId;
        memcpy(&m_data, &id, sizeof(Qt3DCore::QNodeId));
    }

    // For textures
    UniformValue(UniformValue::Texture t)
    {
        m_valueType = TextureValue;
        memcpy(&m_data, &t, sizeof(Texture));
    }

    ValueType valueType() const { return m_valueType; }

    static UniformValue fromVariant(const QVariant &variant);

    template<typename T>
    const T *constData() const
    {
        return reinterpret_cast<const T *>(&m_data);
    }

    template<typename T>
    T *data()
    {
        return reinterpret_cast<T *>(&m_data);
    }

    bool operator==(const UniformValue &other) const
    {
        return memcmp(&m_data, &other.m_data, sizeof(u_Uniform)) == 0;
    }

    bool operator!=(const UniformValue &other) const
    {
        return !(*this == other);
    }
private:
    union u_Uniform {
        int ivec[4]; // store uint/ints/bools
        float fvec[16]; // for matrix4 (note: we could have a special case for matrices)

        u_Uniform()
        {
            memset(this, 0, sizeof(u_Uniform));
        }
    } m_data;

    ValueType m_valueType = ScalarValue;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_UNIFORM_P_H
