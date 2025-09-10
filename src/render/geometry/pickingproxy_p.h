// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_PICKINGPROXY_H
#define QT3DRENDER_RENDER_PICKINGPROXY_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qpickingproxy.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class PickingProxyManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT PickingProxy : public BackendNode
{
public:
    PickingProxy();
    ~PickingProxy();

    void cleanup();
    void setManager(PickingProxyManager *manager);
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    inline Qt3DCore::QNodeId geometryId() const { return m_geometryId; }
    inline int instanceCount() const { return m_instanceCount; }
    inline int vertexCount() const { return m_vertexCount; }
    inline int indexOffset() const { return m_indexOffset; }
    inline int firstInstance() const { return m_firstInstance; }
    inline int firstVertex() const { return m_firstVertex; }
    inline int indexBufferByteOffset() const { return m_indexBufferByteOffset; }
    inline int restartIndexValue() const { return m_restartIndexValue; }
    inline int verticesPerPatch() const { return m_verticesPerPatch; }
    inline bool primitiveRestartEnabled() const { return m_primitiveRestartEnabled; }
    inline Qt3DCore::QGeometryView::PrimitiveType primitiveType() const { return m_primitiveType; }
    inline bool isValid() const { return m_valid; }
    inline bool isDirty() const { return m_dirty; }
    void unsetDirty();

private:
    Qt3DCore::QNodeId m_geometryId;
    int m_instanceCount;
    int m_vertexCount;
    int m_indexOffset;
    int m_firstInstance;
    int m_firstVertex;
    int m_indexBufferByteOffset;
    int m_restartIndexValue;
    int m_verticesPerPatch;
    bool m_primitiveRestartEnabled;
    bool m_dirty;
    bool m_valid;
    Qt3DCore::QGeometryView::PrimitiveType m_primitiveType;
    PickingProxyManager *m_manager;
};

class PickingProxyFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit PickingProxyFunctor(AbstractRenderer *renderer, PickingProxyManager *manager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;
private:
    PickingProxyManager *m_manager;
    AbstractRenderer *m_renderer;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_PICKINGPROXY_H
