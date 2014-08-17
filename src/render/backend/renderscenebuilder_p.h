/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_RENDERSCENEBUILDER_H
#define QT3D_RENDER_RENDERSCENEBUILDER_H

#include <Qt3DCore/nodevisitor.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/framegraphmanager_p.h>
#include <Qt3DCore/qhandle.h>
#include <Qt3DCore/qentity.h>
#include <QStack>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QFrameGraphItem;
class QFrameGraph;

namespace Render {
class RenderEntity;
class FrameGraphNode;
}

typedef QHandle<Render::RenderEntity, 16> HRenderNode;
typedef QHandle<Render::FrameGraphNode, 8> HFrameGraphNode;

namespace Render {

class Renderer;

class RenderSceneBuilder : public NodeVisitor
{
public:
    explicit RenderSceneBuilder(Renderer *renderer);

    RenderEntity *rootNode() const;
    HRenderNode rootHandle() const { return m_rootNodeHandle; }
    void        initializeFrameGraph();
    void        createRenderElement(QNode *frontend);
    void        releaseRenderElement(QNode *frontend);

protected:
    void visitNode(QNode *nd) Q_DECL_OVERRIDE;

private:
    Renderer *m_renderer;
    HRenderNode m_rootNodeHandle;
    QStack<HRenderNode> m_nodeStack;
    HRenderNode m_frameGraphEntityNode;
    QFrameGraph *m_frameGraphRoot;

    void buildFrameGraph(QFrameGraphItem *root);
    Render::FrameGraphNode* backendFrameGraphNode(QNode *node);


    template<typename Backend, typename Frontend>
    Backend *createBackendFrameGraphNode(QNode *n)
    {
        Frontend *f = qobject_cast<Frontend *>(n);
        if (n != Q_NULLPTR) {
            HFrameGraphNode handle = m_renderer->frameGraphManager()->lookupHandle(n->uuid());
            if (handle.isNull()) {
                handle = m_renderer->frameGraphManager()->getOrAcquireHandle(n->uuid());
                Backend *backend = new Backend();
                *m_renderer->frameGraphManager()->data(handle) = backend;
                backend->setRenderer(m_renderer);
                backend->setHandle(handle);
                backend->setPeer(f);
                return backend;
            }
            return static_cast<Backend *>(*m_renderer->frameGraphManager()->data(handle));
        }
        return Q_NULLPTR;
    }

    template<class Frontend, class Backend, class Manager>
    Backend *createRenderElementHelper(QNode *frontend, Manager *manager)
    {
        // We index using the Frontend uuid
        if (!manager->contains(frontend->uuid())) {
            Backend *backend = manager->getOrCreateResource(frontend->uuid());
            backend->setRenderer(m_renderer);
            backend->setPeer(qobject_cast<Frontend *>(frontend));
            return backend;
        }
        return manager->lookupResource(frontend->uuid());
    }

    RenderEntity* createRenderNode(QEntity *node);
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERSCENEBUILDER_H
