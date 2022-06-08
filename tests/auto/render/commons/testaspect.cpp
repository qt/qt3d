// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "testaspect.h"
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QList<Qt3DCore::QNode *> getNodesForCreation(Qt3DCore::QNode *root)
{
    using namespace Qt3DCore;

    QList<QNode *> nodes;
    Qt3DCore::QNodeVisitor visitor;
    visitor.traverse(root, [&nodes](QNode *node) {
        nodes.append(node);

        // Store the metaobject of the node in the QNode so that we have it available
        // to us during destruction in the QNode destructor. This allows us to send
        // the QNodeId and the metaobject as typeinfo to the backend aspects so they
        // in turn can find the correct QBackendNodeMapper object to handle the destruction
        // of the corresponding backend nodes.
        QNodePrivate *d = QNodePrivate::get(node);
        d->m_typeInfo = const_cast<QMetaObject*>(QNodePrivate::findStaticMetaObject(node->metaObject()));

        // Mark this node as having been handled for creation so that it is picked up
        d->m_hasBackendNode = true;
    });

    return nodes;
}

TestAspect::TestAspect(Qt3DCore::QNode *root)
    : Qt3DRender::QRenderAspect()
    , m_jobManager(new Qt3DCore::QAspectJobManager())
{
    Qt3DCore::QAbstractAspectPrivate::get(this)->m_jobManager = m_jobManager.data();
    QRenderAspect::onRegistered();

    const QList<Qt3DCore::QNode *> nodes = getNodesForCreation(root);

    for (Qt3DCore::QNode *node : nodes)
        d_func()->createBackendNode({
                                        node->id(),
                                        Qt3DCore::QNodePrivate::get(node)->m_typeInfo,
                                        Qt3DCore::NodeTreeChange::Added,
                                        node
                                    });
}

TestAspect::~TestAspect()
{
    QRenderAspect::onUnregistered();
}

Render::NodeManagers *TestAspect::nodeManagers() const
{
    return d_func()->m_renderer->nodeManagers();
}

void TestAspect::onEngineStartup()
{
    QRenderAspect::onEngineStartup();
}

void TestAspect::onRegistered()
{
    QRenderAspect::onRegistered();
}

void TestAspect::onUnregistered()
{
    QRenderAspect::onUnregistered();
}

} // Qt3DRender

QT_END_NAMESPACE
