// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_FRAMEGRAPHNODE_H
#define QT3DRENDER_RENDER_FRAMEGRAPHNODE_H

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
#include <Qt3DCore/private/qhandle_p.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <qglobal.h>
#include <QList>

// Windows had the smart idea of using a #define MemoryBarrier
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684208(v=vs.85).aspx
#if defined(Q_OS_WIN) && defined(MemoryBarrier)
#undef MemoryBarrier
#endif

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class FrameGraphManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT FrameGraphNode : public BackendNode
{
public:
    FrameGraphNode();
    virtual ~FrameGraphNode();

    enum FrameGraphNodeType {
        InvalidNodeType = 0,
        CameraSelector,
        LayerFilter,
        RenderPassFilter,
        RenderTarget,
        TechniqueFilter,
        Viewport,
        ClearBuffers,
        SortMethod,
        SubtreeEnabler,
        StateSet,
        NoDraw,
        FrustumCulling,
        Lighting,
        ComputeDispatch,
        Surface,
        RenderCapture,
        BufferCapture,
        MemoryBarrier,
        ProximityFilter,
        BlitFramebuffer,
        SetFence,
        WaitFence,
        NoPicking,
        DebugOverlay,
    };
    FrameGraphNodeType nodeType() const { return m_nodeType; }

    void setFrameGraphManager(FrameGraphManager *manager);
    FrameGraphManager *manager() const;

    void setParentId(Qt3DCore::QNodeId parentId);

    Qt3DCore::QNodeId parentId() const;
    QList<Qt3DCore::QNodeId> childrenIds() const;

    FrameGraphNode *parent() const;
    QList<FrameGraphNode *> children() const;

    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

protected:
    FrameGraphNode(FrameGraphNodeType nodeType, QBackendNode::Mode mode = QBackendNode::ReadOnly);

private:
    FrameGraphNodeType m_nodeType;
    Qt3DCore::QNodeId m_parentId;
    QList<Qt3DCore::QNodeId> m_childrenIds;
    FrameGraphManager *m_manager;

    friend class FrameGraphVisitor;
};

template<typename Backend, typename Frontend>
class FrameGraphNodeFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit FrameGraphNodeFunctor(AbstractRenderer *renderer)
        : m_manager(renderer->nodeManagers()->frameGraphManager())
        , m_renderer(renderer)
    {
    }

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override
    {
        return createBackendFrameGraphNode(id);
    }

    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override
    {
        return m_manager->lookupNode(id);
    }

    void destroy(Qt3DCore::QNodeId id) const override
    {
        m_manager->releaseNode(id);
    }

protected:
    Backend *createBackendFrameGraphNode(const Qt3DCore::QNodeId &id) const
    {
        if (!m_manager->containsNode(id)) {
            Backend *backend = new Backend();
            backend->setFrameGraphManager(m_manager);
            backend->setRenderer(m_renderer);
            m_manager->appendNode(id, backend);
            return backend;
        }
        return static_cast<Backend *>(m_manager->lookupNode(id));
    }

private:
    FrameGraphManager *m_manager;
    AbstractRenderer *m_renderer;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FRAMEGRAPHNODE_H
