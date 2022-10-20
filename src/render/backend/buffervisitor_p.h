/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_BUFFERVISITOR_P_H
#define QT3DRENDER_RENDER_BUFFERVISITOR_P_H

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
#include <Qt3DCore/private/buffervisitor_p.h>
#include <Qt3DRender/private/trianglesvisitor_p.h>
#include <Qt3DRender/private/attribute_p.h>
#include <Qt3DRender/private/buffer_p.h>
#include <Qt3DRender/private/bufferutils_p.h>
#include <Qt3DRender/private/geometryrenderer_p.h>
#include <Qt3DRender/private/geometry_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {
namespace Render {


template <typename ValueType, Qt3DCore::QAttribute::VertexBaseType VertexBaseType, uint dataSize>
class Q_AUTOTEST_EXPORT BufferVisitor : public Qt3DCore::BufferVisitor<ValueType, VertexBaseType, dataSize>
{
public:
    explicit BufferVisitor(NodeManagers *manager)
        : m_manager(manager)
    {
    }
    virtual ~BufferVisitor() = default;

    bool apply(Qt3DRender::Render::Attribute *attribute,
               Qt3DRender::Render::Attribute *indexAttribute,
               int drawVertexCount,
               bool primitiveRestartEnabled,
               int primitiveRestartIndex)
    {
        if (attribute->vertexBaseType() != VertexBaseType)
            return false;
        if (attribute->vertexSize() < dataSize)
            return false;

        auto data = m_manager->lookupResource<Buffer, BufferManager>(attribute->bufferId())->data();
        auto vertexBuffer = BufferTypeInfo::castToType<VertexBaseType>(data, attribute->byteOffset());

        if (indexAttribute) {
            auto indexData = m_manager->lookupResource<Buffer, BufferManager>(indexAttribute->bufferId())->data();
            switch (indexAttribute->vertexBaseType()) {
            case Qt3DCore::QAttribute::UnsignedShort: {
                auto indexBuffer = BufferTypeInfo::castToType<Qt3DCore::QAttribute::UnsignedShort>(indexData, indexAttribute->byteOffset());
                this->traverseCoordinateIndexed(vertexBuffer, indexBuffer, attribute->byteStride(), drawVertexCount,
                                          primitiveRestartEnabled, primitiveRestartIndex);
                break;
            }
            case Qt3DCore::QAttribute::UnsignedInt: {
                auto indexBuffer = BufferTypeInfo::castToType<Qt3DCore::QAttribute::UnsignedInt>(indexData, indexAttribute->byteOffset());
                this->traverseCoordinateIndexed(vertexBuffer, indexBuffer, attribute->byteStride(), drawVertexCount,
                                          primitiveRestartEnabled, primitiveRestartIndex);
                break;
            }
            case Qt3DCore::QAttribute::UnsignedByte: {
                auto indexBuffer = BufferTypeInfo::castToType<Qt3DCore::QAttribute::UnsignedByte>(indexData, indexAttribute->byteOffset());
                this->traverseCoordinateIndexed(vertexBuffer, indexBuffer, attribute->byteStride(), drawVertexCount,
                                          primitiveRestartEnabled, primitiveRestartIndex);
                break;
            }
            default: Q_UNREACHABLE();
            }
        } else {
            switch (dataSize) {
            case 1: this->traverseCoordinates1(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            case 2: this->traverseCoordinates2(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            case 3: this->traverseCoordinates3(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            case 4: this->traverseCoordinates4(vertexBuffer, attribute->byteStride(), drawVertexCount); break;
            default: Q_UNREACHABLE();
            }
        }

        return true;
    }

protected:
    NodeManagers *m_manager;
};

typedef BufferVisitor<float, Qt3DCore::QAttribute::Float, 3> Buffer3fVisitor;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_BUFFERVISITOR_P_H
