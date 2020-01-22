/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
