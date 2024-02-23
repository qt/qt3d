// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef TESTRENDERASPECT_H
#define TESTRENDERASPECT_H

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qabstractfrontendnodemanager_p.h>
#include <Qt3DRender/private/qrenderaspect_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class TestAspect : public Qt3DRender::QRenderAspect, public Qt3DCore::QAbstractFrontEndNodeManager
{
public:
    TestAspect(Qt3DCore::QNode *root);

    ~TestAspect();

    Qt3DRender::Render::NodeManagers *nodeManagers() const;

    void onEngineStartup() override;
    void onRegistered() override;
    void onUnregistered() override;

    void registerNode(Qt3DCore::QNode *node) { m_frontEndNodes.insert(node->id(), node); }
    void registerTree(Qt3DCore::QEntity *root) {
        using namespace Qt3DCore;
        QNodeVisitor visitor;
        visitor.traverse(root, [](QNode *) {}, [this](QEntity *entity) {
                registerNode(entity);
                const auto &components = entity->components();
                for (const auto &c : components)
                    registerNode(c);
            });
    }
    Qt3DCore::QNode *lookupNode(Qt3DCore::QNodeId id) const override { return  m_frontEndNodes.value(id, nullptr); }
    QList<Qt3DCore::QNode *> lookupNodes(const QList<Qt3DCore::QNodeId> &ids) const override {
        QList<Qt3DCore::QNode *> res;
        for (const auto &id: ids) {
            auto node = m_frontEndNodes.value(id, nullptr);
            if (node)
                res.push_back(node);
        }
        return  res;
    }

private:
    QScopedPointer<Qt3DCore::QAspectJobManager> m_jobManager;
    QHash<Qt3DCore::QNodeId, Qt3DCore::QNode *> m_frontEndNodes;
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // TESTRENDERASPECT_H
