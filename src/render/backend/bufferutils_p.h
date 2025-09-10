// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_BUFFERUTILS_P_H
#define QT3DRENDER_RENDER_BUFFERUTILS_P_H

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

#include <Qt3DCore/QAttribute>
#include <QByteArray>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {

namespace Render {

class GeometryRenderer;
class NodeManagers;
class Attribute;
class Buffer;

struct BufferInfo
{
    BufferInfo()
        : type(Qt3DCore::QAttribute::VertexBaseType::Float)
        , dataSize(0)
        , count(0)
        , byteStride(0)
        , byteOffset(0)
        , restartEnabled(false)
        , restartIndexValue(-1)
    {}

    QByteArray data;
    Qt3DCore::QAttribute::VertexBaseType type;
    uint dataSize;
    uint count;
    uint byteStride;
    uint byteOffset;
    bool restartEnabled;
    int restartIndexValue;
};


namespace BufferTypeInfo {

    template <Qt3DCore::QAttribute::VertexBaseType> struct EnumToType;
    template <> struct EnumToType<Qt3DCore::QAttribute::Byte> { typedef const char type; };
    template <> struct EnumToType<Qt3DCore::QAttribute::UnsignedByte> { typedef const uchar type; };
    template <> struct EnumToType<Qt3DCore::QAttribute::Short> { typedef const short type; };
    template <> struct EnumToType<Qt3DCore::QAttribute::UnsignedShort> { typedef const ushort type; };
    template <> struct EnumToType<Qt3DCore::QAttribute::Int> { typedef const int type; };
    template <> struct EnumToType<Qt3DCore::QAttribute::UnsignedInt> { typedef const uint type; };
    template <> struct EnumToType<Qt3DCore::QAttribute::Float> { typedef const float type; };
    template <> struct EnumToType<Qt3DCore::QAttribute::Double> { typedef const double type; };

    template<Qt3DCore::QAttribute::VertexBaseType v>
    typename EnumToType<v>::type *castToType(const QByteArray &u, uint byteOffset)
    {
        return reinterpret_cast< typename EnumToType<v>::type *>(u.constData() + byteOffset);
    }

} // namespace BufferTypeInfo

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_BUFFERUTILS_P_H
