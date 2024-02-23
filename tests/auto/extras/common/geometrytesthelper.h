// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef GEOMETRYTESTHELPER_H
#define GEOMETRYTESTHELPER_H

#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qgeometry.h>

template<typename IndexType>
IndexType extractIndexData(Qt3DCore::QAttribute *attribute, int index)
{
    // Get the raw data
    const IndexType *typedData = reinterpret_cast<const IndexType *>(attribute->buffer()->data().constData());

    // Offset into the data taking stride and offset into account
    const IndexType indexValue = *(typedData + index);
    return indexValue;
}

template<typename VertexType, typename IndexType>
VertexType extractVertexData(Qt3DCore::QAttribute *attribute, IndexType index)
{
    // Get the raw data
    const char *rawData = attribute->buffer()->data().constData();

    // Offset into the data taking stride and offset into account
    const char *vertexData = rawData + (index * attribute->byteStride() + attribute->byteOffset());

    // Construct vertex from the typed data
    VertexType vertex;
    const Qt3DCore::QAttribute::VertexBaseType type = attribute->vertexBaseType();
    switch (type)
    {
    case Qt3DCore::QAttribute::Float: {
        const float *typedVertexData = reinterpret_cast<const float *>(vertexData);
        const int components = attribute->vertexSize();
        for (int i = 0; i < components; ++i)
            vertex[i] = typedVertexData[i];
        break;

        // TODO: Handle other types as needed
    }

    default:
        qWarning() << "Unhandled type";
        Q_UNREACHABLE();
        break;
    }

    return vertex;
}

#endif // GEOMETRYTESTHELPER_H
