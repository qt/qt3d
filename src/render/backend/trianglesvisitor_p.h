// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_TRIANGLESVISITOR_P_H
#define QT3DRENDER_RENDER_TRIANGLESVISITOR_P_H

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
#include <Qt3DRender/private/bufferutils_p.h>
#include <Qt3DCore/private/vector3d_p.h>
#include <Qt3DCore/private/vector4d_p.h>

#include <private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {

namespace Render {

class GeometryRenderer;
class PickingProxy;
class NodeManagers;
class Attribute;
class Buffer;

class Q_AUTOTEST_EXPORT TrianglesVisitor
{
public:
    explicit TrianglesVisitor(NodeManagers *manager) : m_manager(manager) { }
    virtual ~TrianglesVisitor();

    void apply(const Qt3DCore::QEntity *entity);
    void apply(const GeometryRenderer *renderer, const Qt3DCore::QNodeId id);
    void apply(const PickingProxy *proxy, const Qt3DCore::QNodeId id);

    virtual void visit(uint andx, const Vector3D &a,
                       uint bndx, const Vector3D &b,
                       uint cndx, const Vector3D &c) = 0;

protected:
    NodeManagers *m_manager;
    Qt3DCore::QNodeId m_nodeId;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT CoordinateReader
{
public:
    explicit CoordinateReader(NodeManagers *manager)
        : m_manager(manager)
        , m_attribute(nullptr)
        , m_buffer(nullptr)
    {
    }

    bool setGeometry(const GeometryRenderer *renderer, const QString &attributeName);

    Vector4D getCoordinate(uint vertexIndex);

protected:
    NodeManagers *m_manager;
    Attribute *m_attribute;
    Buffer *m_buffer;
    BufferInfo m_bufferInfo;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_TRIANGLESVISITOR_P_H
