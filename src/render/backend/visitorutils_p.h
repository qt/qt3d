// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_VISITORUTILS_P_H
#define QT3DRENDER_RENDER_VISITORUTILS_P_H

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

#include <QtGlobal>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Visitor {

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
inline typename EnumToType<v>::type *castToType(const QByteArray &u, uint byteOffset)
{
    return reinterpret_cast< typename EnumToType<v>::type *>(u.constData() + byteOffset);
}

template<typename Func>
void processBuffer(const BufferInfo &info, Func &f)
{
    switch (info.type) {
    case Qt3DCore::QAttribute::Byte: f(info, castToType<Qt3DCore::QAttribute::Byte>(info.data, info.byteOffset));
        return;
    case Qt3DCore::QAttribute::UnsignedByte: f(info, castToType<Qt3DCore::QAttribute::UnsignedByte>(info.data, info.byteOffset));
        return;
    case Qt3DCore::QAttribute::Short: f(info, castToType<Qt3DCore::QAttribute::Short>(info.data, info.byteOffset));
        return;
    case Qt3DCore::QAttribute::UnsignedShort: f(info, castToType<Qt3DCore::QAttribute::UnsignedShort>(info.data, info.byteOffset));
        return;
    case Qt3DCore::QAttribute::Int: f(info, castToType<Qt3DCore::QAttribute::Int>(info.data, info.byteOffset));
        return;
    case Qt3DCore::QAttribute::UnsignedInt: f(info, castToType<Qt3DCore::QAttribute::UnsignedInt>(info.data, info.byteOffset));
        return;
    case Qt3DCore::QAttribute::Float: f(info, castToType<Qt3DCore::QAttribute::Float>(info.data, info.byteOffset));
        return;
    case Qt3DCore::QAttribute::Double: f(info, castToType<Qt3DCore::QAttribute::Double>(info.data, info.byteOffset));
        return;
    default:
        return;
    }
}

template<typename GeometryProvider, typename VertexExecutor, typename IndexExecutor, typename Visitor>
void visitPrimitives(NodeManagers *manager, const GeometryProvider *renderer, Visitor* visitor)
{
    Geometry *geom = manager->lookupResource<Geometry, GeometryManager>(renderer->geometryId());
    Attribute *positionAttribute = nullptr;
    Attribute *indexAttribute = nullptr;
    Buffer *positionBuffer = nullptr;
    Buffer *indexBuffer = nullptr;

    auto updateStride = [](BufferInfo &info, int stride) {
        if (stride) {
            info.byteStride = stride;
            return;
        }
        switch (info.type) {
        case Qt3DCore::QAttribute::Byte: info.byteStride = sizeof(qint8) * info.dataSize; return;
        case Qt3DCore::QAttribute::UnsignedByte: info.byteStride = sizeof(quint8) * info.dataSize; return;
        case Qt3DCore::QAttribute::Short: info.byteStride = sizeof(qint16) * info.dataSize; return;
        case Qt3DCore::QAttribute::UnsignedShort: info.byteStride = sizeof(quint16) * info.dataSize; return;
        case Qt3DCore::QAttribute::Int: info.byteStride = sizeof(qint32) * info.dataSize; return;
        case Qt3DCore::QAttribute::UnsignedInt: info.byteStride = sizeof(quint32) * info.dataSize; return;
        case Qt3DCore::QAttribute::Float: info.byteStride = sizeof(float) * info.dataSize; return;
        case Qt3DCore::QAttribute::Double: info.byteStride = sizeof(double) * info.dataSize; return;
        default: return;
        }
    };

    if (geom) {
        Qt3DRender::Render::Attribute *attribute = nullptr;
        const auto attrIds = geom->attributes();
        for (const Qt3DCore::QNodeId &attrId : attrIds) {
            attribute = manager->lookupResource<Attribute, AttributeManager>(attrId);
            if (attribute){
                if (!positionAttribute && attribute->name() == Qt3DCore::QAttribute::defaultPositionAttributeName())
                    positionAttribute = attribute;
                else if (attribute->attributeType() == Qt3DCore::QAttribute::IndexAttribute)
                    indexAttribute = attribute;
            }
        }

        if (positionAttribute)
            positionBuffer = manager->lookupResource<Buffer, BufferManager>(positionAttribute->bufferId());
        if (indexAttribute)
            indexBuffer = manager->lookupResource<Buffer, BufferManager>(indexAttribute->bufferId());

        if (positionBuffer) {

            BufferInfo vertexBufferInfo;
            vertexBufferInfo.data = positionBuffer->data();
            vertexBufferInfo.type = positionAttribute->vertexBaseType();
            vertexBufferInfo.byteOffset = positionAttribute->byteOffset();
            vertexBufferInfo.dataSize = positionAttribute->vertexSize();
            vertexBufferInfo.count = positionAttribute->count();
            updateStride(vertexBufferInfo, positionAttribute->byteStride());

            if (indexBuffer) { // Indexed

                BufferInfo indexBufferInfo;
                indexBufferInfo.data = indexBuffer->data();
                indexBufferInfo.type = indexAttribute->vertexBaseType();
                indexBufferInfo.byteOffset = indexAttribute->byteOffset();
                indexBufferInfo.count = indexAttribute->count();
                indexBufferInfo.restartEnabled = renderer->primitiveRestartEnabled();
                indexBufferInfo.restartIndexValue = renderer->restartIndexValue();
                updateStride(indexBufferInfo, indexAttribute->byteStride());

                IndexExecutor executor;
                executor.m_vertexBufferInfo = vertexBufferInfo;
                executor.m_primitiveType = static_cast<Qt3DRender::QGeometryRenderer::PrimitiveType>(renderer->primitiveType());
                executor.m_visitor = visitor;

                return processBuffer(indexBufferInfo, executor);

            } else { // Non Indexed

                // Check into which type the buffer needs to be casted
                VertexExecutor executor;
                executor.m_primitiveType = static_cast<Qt3DRender::QGeometryRenderer::PrimitiveType>(renderer->primitiveType());
                executor.m_visitor = visitor;

                return processBuffer(vertexBufferInfo, executor);
            }
        }
    }
}

} // namespace Visitor

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_VISITORUTILS_P_H
