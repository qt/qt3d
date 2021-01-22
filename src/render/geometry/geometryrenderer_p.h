/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_GEOMETRYRENDERER_H
#define QT3DRENDER_RENDER_GEOMETRYRENDERER_H

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
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qgeometryfactory.h>
#include <Qt3DRender/qmesh.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {
class QBoundingVolume;
}

namespace Render {

class GeometryRendererManager;

struct GeometryFunctorResult
{
    QGeometry *geometry;
    QMesh::Status status;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT GeometryRenderer : public BackendNode
{
public:
    GeometryRenderer();
    ~GeometryRenderer();

    void cleanup();
    void setManager(GeometryRendererManager *manager);
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    GeometryFunctorResult executeFunctor();

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
    inline QGeometryRenderer::PrimitiveType primitiveType() const { return m_primitiveType; }
    inline bool isDirty() const { return m_dirty; }
    inline QGeometryFactoryPtr geometryFactory() const { return m_geometryFactory; }
    void unsetDirty();

    // Build triangle data Job thread
    void setTriangleVolumes(const  QVector<RayCasting::QBoundingVolume *> &volumes);
    // Pick volumes job
    QVector<RayCasting::QBoundingVolume *> triangleData() const;

    float sortIndex() const { return m_sortIndex; }

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
    QGeometryRenderer::PrimitiveType m_primitiveType;
    bool m_dirty;
    QGeometryFactoryPtr m_geometryFactory;
    GeometryRendererManager *m_manager;
    QVector<RayCasting::QBoundingVolume *> m_triangleVolumes;
    float m_sortIndex;
};

class GeometryRendererFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit GeometryRendererFunctor(AbstractRenderer *renderer, GeometryRendererManager *manager);
    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;
private:
    GeometryRendererManager *m_manager;
    AbstractRenderer *m_renderer;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERGEOMETRYRENDERER_H
