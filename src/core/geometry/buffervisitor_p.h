// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_BUFFERVISITOR_P_H
#define QT3DCORE_BUFFERVISITOR_P_H

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

#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/private/bufferutils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;

template <typename ValueType, Qt3DCore::QAttribute::VertexBaseType VertexBaseType, uint dataSize>
class Q_AUTOTEST_EXPORT BufferVisitor
{
public:
    explicit BufferVisitor() = default;
    virtual ~BufferVisitor() = default;

    virtual void visit(uint ndx, ValueType x) {
        Q_UNUSED(ndx); Q_UNUSED(x);
    }
    virtual void visit(uint ndx, ValueType x, ValueType y) {
        Q_UNUSED(ndx); Q_UNUSED(x); Q_UNUSED(y);
    }
    virtual void visit(uint ndx, ValueType x, ValueType y, ValueType z) {
        Q_UNUSED(ndx); Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(z);
    }
    virtual void visit(uint ndx, ValueType x, ValueType y, ValueType z, ValueType w) {
        Q_UNUSED(ndx); Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(z); Q_UNUSED(w);
    }

    bool apply(QAttribute *attribute,
               QAttribute *indexAttribute,
               int drawVertexCount,
               bool primitiveRestartEnabled,
               int primitiveRestartIndex)
    {
        if (attribute->vertexBaseType() != VertexBaseType)
            return false;
        if (attribute->vertexSize() < dataSize)
            return false;

        auto data = attribute->buffer()->data();
        auto vertexBuffer = BufferTypeInfo::castToType<VertexBaseType>(data, attribute->byteOffset());

        if (indexAttribute) {
            auto indexData = indexAttribute->buffer()->data();
            switch (indexAttribute->vertexBaseType()) {
            case Qt3DCore::QAttribute::UnsignedShort: {
                auto indexBuffer = BufferTypeInfo::castToType<Qt3DCore::QAttribute::UnsignedShort>(indexData, indexAttribute->byteOffset());
                traverseCoordinateIndexed(vertexBuffer, indexBuffer, attribute->byteStride(), drawVertexCount,
                                          primitiveRestartEnabled, primitiveRestartIndex);
                break;
            }
            case Qt3DCore::QAttribute::UnsignedInt: {
                auto indexBuffer = BufferTypeInfo::castToType<Qt3DCore::QAttribute::UnsignedInt>(indexData, indexAttribute->byteOffset());
                traverseCoordinateIndexed(vertexBuffer, indexBuffer, attribute->byteStride(), drawVertexCount,
                                          primitiveRestartEnabled, primitiveRestartIndex);
                break;
            }
            case Qt3DCore::QAttribute::UnsignedByte: {
                auto indexBuffer = BufferTypeInfo::castToType<Qt3DCore::QAttribute::UnsignedByte>(indexData, indexAttribute->byteOffset());
                traverseCoordinateIndexed(vertexBuffer, indexBuffer, attribute->byteStride(), drawVertexCount,
                                          primitiveRestartEnabled, primitiveRestartIndex);
                break;
            }
            default: Q_UNREACHABLE();
            }
        } else {
            switch (dataSize) {
            case 1: traverseCoordinates1(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            case 2: traverseCoordinates2(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            case 3: traverseCoordinates3(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            case 4: traverseCoordinates4(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            default: Q_UNREACHABLE();
            }
        }

        return true;
    }

protected:
    template<typename VertexBufferType, typename IndexBufferType>
    void traverseCoordinateIndexed(VertexBufferType *vertexBuffer,
                                   IndexBufferType *indexBuffer,
                                   int vertexByteStride,
                                   int drawVertexCount,
                                   bool primitiveRestartEnabled,
                                   int primitiveRestartIndex)
    {
        switch (dataSize) {
        case 1: traverseCoordinates1Indexed(vertexBuffer, vertexByteStride, indexBuffer, drawVertexCount,
                                        primitiveRestartEnabled, primitiveRestartIndex);
            break;
        case 2: traverseCoordinates2Indexed(vertexBuffer, vertexByteStride, indexBuffer, drawVertexCount,
                                        primitiveRestartEnabled, primitiveRestartIndex);
            break;
        case 3: traverseCoordinates3Indexed(vertexBuffer, vertexByteStride, indexBuffer, drawVertexCount,
                                        primitiveRestartEnabled, primitiveRestartIndex);
            break;
        case 4: traverseCoordinates4Indexed(vertexBuffer, vertexByteStride, indexBuffer, drawVertexCount,
                                        primitiveRestartEnabled, primitiveRestartIndex);
            break;
        default: Q_UNREACHABLE();
        }
    }

    template <typename Coordinate>
    void traverseCoordinates1(Coordinate *coordinates,
                              const uint byteStride,
                              const uint count)
    {
        const uint stride = byteStride / sizeof(Coordinate);
        for (uint ndx = 0; ndx < count; ++ndx) {
            visit(ndx, coordinates[0]);
            coordinates += stride;
        }
    }

    template <typename Coordinate, typename IndexElem>
    void traverseCoordinates1Indexed(Coordinate *coordinates,
                                     const uint byteStride,
                                     IndexElem *indices,
                                     const uint count,
                                     bool primitiveRestartEnabled,
                                     int primitiveRestartIndex)
    {
        const uint stride = byteStride / sizeof(Coordinate);
        if (primitiveRestartEnabled) {
            for (uint i = 0; i < count; ++i) {
                if (static_cast<int>(indices[i]) != primitiveRestartIndex) {
                    const uint n = stride * indices[i];
                    visit(i, coordinates[n]);
                }
            }
        } else {
            for (uint i = 0; i < count; ++i) {
                const uint n = stride * indices[i];
                visit(i, coordinates[n]);
            }
        }
    }

    template <typename Coordinate>
    void traverseCoordinates2(Coordinate *coordinates,
                              const uint byteStride,
                              const uint count)
    {
        const uint stride = byteStride ? byteStride / sizeof(Coordinate) : 2;
        for (uint ndx = 0; ndx < count; ++ndx) {
            visit(ndx, coordinates[0], coordinates[1]);
            coordinates += stride;
        }
    }

    template <typename Coordinate, typename IndexElem>
    void traverseCoordinates2Indexed(Coordinate *coordinates,
                                     const uint byteStride,
                                     IndexElem *indices,
                                     const uint count,
                                     bool primitiveRestartEnabled,
                                     int primitiveRestartIndex)
    {
        const uint stride = byteStride ? byteStride / sizeof(Coordinate) : 2;
        if (primitiveRestartEnabled) {
            for (uint i = 0; i < count; ++i) {
                if (static_cast<int>(indices[i]) != primitiveRestartIndex) {
                    const uint n = stride * indices[i];
                    visit(i, coordinates[n], coordinates[n + 1]);
                }
            }
        } else {
            for (uint i = 0; i < count; ++i) {
                const uint n = stride * indices[i];
                visit(i, coordinates[n], coordinates[n + 1]);
            }
        }
    }

    template <typename Coordinate>
    void traverseCoordinates3(Coordinate *coordinates,
                              const uint byteStride,
                              const uint count)
    {
        const uint stride = byteStride ? byteStride / sizeof(Coordinate) : 3;
        for (uint ndx = 0; ndx < count; ++ndx) {
            visit(ndx, coordinates[0], coordinates[1], coordinates[2]);
            coordinates += stride;
        }
    }

    template <typename Coordinate, typename IndexElem>
    void traverseCoordinates3Indexed(Coordinate *coordinates,
                                     const uint byteStride,
                                     IndexElem *indices,
                                     const uint count,
                                     bool primitiveRestartEnabled,
                                     int primitiveRestartIndex)
    {
        const uint stride = byteStride ? byteStride / sizeof(Coordinate) : 3;
        if (primitiveRestartEnabled) {
            for (uint i = 0; i < count; ++i) {
                if (static_cast<int>(indices[i]) != primitiveRestartIndex) {
                    const uint n = stride * indices[i];
                    visit(i, coordinates[n], coordinates[n + 1], coordinates[n + 2]);
                }
            }
        } else {
            for (uint i = 0; i < count; ++i) {
                const uint n = stride * indices[i];
                visit(i, coordinates[n], coordinates[n + 1], coordinates[n + 2]);
            }
        }
    }

    template <typename Coordinate>
    void traverseCoordinates4(Coordinate *coordinates,
                              const uint byteStride,
                              const uint count)
    {
        const uint stride = byteStride ? byteStride / sizeof(Coordinate) : 4;
        for (uint ndx = 0; ndx < count; ++ndx) {
            visit(ndx, coordinates[0], coordinates[1], coordinates[2], coordinates[3]);
            coordinates += stride;
        }
    }

    template <typename Coordinate, typename IndexElem>
    void traverseCoordinates4Indexed(Coordinate *coordinates,
                                     const uint byteStride,
                                     IndexElem *indices,
                                     const uint count,
                                     bool primitiveRestartEnabled,
                                     int primitiveRestartIndex)
    {
        const uint stride = byteStride ? byteStride / sizeof(Coordinate) : 4;
        if (primitiveRestartEnabled) {
            for (uint i = 0; i < count; ++i) {
                if (static_cast<int>(indices[i]) != primitiveRestartIndex) {
                    const uint n = stride * indices[i];
                    visit(i, coordinates[n], coordinates[n + 1], coordinates[n + 2], coordinates[n + 3]);
                }
            }
        } else {
            for (uint i = 0; i < count; ++i) {
                const uint n = stride * indices[i];
                visit(i, coordinates[n], coordinates[n + 1], coordinates[n + 2], coordinates[n + 3]);
            }
        }
    }
};

typedef BufferVisitor<float, Qt3DCore::QAttribute::Float, 3> Buffer3fVisitor;

} // namespace Qt3DCore

QT_END_NAMESPACE


#endif // QT3DCORE_BUFFERVISITOR_P_H
