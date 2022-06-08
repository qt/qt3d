// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DANIMATION_ANIMATION_NODEFUNCTOR_P_H
#define QT3DANIMATION_ANIMATION_NODEFUNCTOR_P_H

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

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qbackendnode.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Handler;

template<class Backend, class Manager>
class NodeFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit NodeFunctor(Handler *handler, Manager *manager)
        : m_handler(handler)
        , m_manager(manager)
    {
    }

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeId nodeId) const final
    {
        Backend *backend = m_manager->getOrCreateResource(nodeId);
        backend->setHandler(m_handler);
        return backend;
    }

    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final
    {
        return m_manager->lookupResource(id);
    }

    void destroy(Qt3DCore::QNodeId id) const final
    {
        m_manager->releaseResource(id);
    }

private:
    Handler *m_handler;
    Manager *m_manager;
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_NODEFUNCTOR_P_H
