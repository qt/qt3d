// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_CLIPBLENDNODE_P_H
#define QT3DANIMATION_ANIMATION_CLIPBLENDNODE_P_H

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
#include <Qt3DAnimation/private/backendnode_p.h>
#include <Qt3DAnimation/private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

namespace Animation {

class ClipBlendNodeManager;

class Q_AUTOTEST_EXPORT ClipBlendNode : public BackendNode
{
public:
    ~ClipBlendNode();

    enum BlendType {
        NoneBlendType,
        LerpBlendType,
        AdditiveBlendType,
        ValueType
    };

    void setClipBlendNodeManager(ClipBlendNodeManager *manager);
    inline ClipBlendNodeManager *clipBlendNodeManager() const { return m_manager; }

    BlendType blendType() const;

    void blend(Qt3DCore::QNodeId animatorId);

    void setClipResults(Qt3DCore::QNodeId animatorId, const ClipResults &clipResults);
    ClipResults clipResults(Qt3DCore::QNodeId animatorId) const;

    virtual QList<Qt3DCore::QNodeId> allDependencyIds() const = 0;
    virtual QList<Qt3DCore::QNodeId> currentDependencyIds() const = 0;
    virtual double duration() const = 0;

protected:
    explicit ClipBlendNode(BlendType blendType);
    virtual ClipResults doBlend(const QList<ClipResults> &blendData) const = 0;

private:
    ClipBlendNodeManager *m_manager;
    BlendType m_blendType;

    // Store the results of evaluations indexed by animator id
    QList<Qt3DCore::QNodeId> m_animatorIds;
    QList<ClipResults> m_clipResults;
};

template<typename Backend, typename Frontend>
class ClipBlendNodeFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit ClipBlendNodeFunctor(Handler *handler, ClipBlendNodeManager *manager)
        : m_handler(handler)
        , m_manager(manager)
    {
    }

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final
    {
        if (m_manager->containsNode(id))
            return static_cast<Backend *>(m_manager->lookupNode(id));
        Backend *backend = new Backend();
        backend->setClipBlendNodeManager(m_manager);
        backend->setHandler(m_handler);
        m_manager->appendNode(id, backend);
        return backend;
    }

    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final
    {
        return m_manager->lookupNode(id);
    }

    void destroy(Qt3DCore::QNodeId id) const final
    {
        m_manager->releaseNode(id);
    }

private:
    Handler *m_handler;
    ClipBlendNodeManager *m_manager;
};

} // Animation

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_CLIPBLENDNODE_P_H
