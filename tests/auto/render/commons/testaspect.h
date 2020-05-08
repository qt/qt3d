/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TESTRENDERASPECT_H
#define TESTRENDERASPECT_H

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/qabstractfrontendnodemanager.h>
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
    QVector<Qt3DCore::QNode *> lookupNodes(const QVector<Qt3DCore::QNodeId> &ids) const override {
        QVector<Qt3DCore::QNode *> res;
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
