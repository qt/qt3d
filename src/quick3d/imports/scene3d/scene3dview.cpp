/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "scene3dview_p.h"
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QViewport>
#include <scene3dsgnode_p.h>
#include <scene3ditem_p.h>
#include <QQuickWindow>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \qmltype Scene3DView
    \inherits Item
    \inqmlmodule QtQuick.Scene3D
    \since 5.14

    \preliminary

    \brief The Scene3DView type is used to integrate a Qt 3D sub scene into a
    QtQuick 2 scene using Scene3D. Whereas you should only use a single Scene3D
    instance per application, you can have multiple Scene3DView instances.

    Essentially, if you need to render multiple scenes each in a separate view,
    you should use a single Scene3D instance and as many Scene3DView items as
    you have scenes to render.

    Typical usage looks like:
    \qml
    Scene3D {
        id: mainScene3D
        anchors.fill: parent
    }

    Scene3DView {
        id: view1
        scene3D: mainScene3D
        width: 200
        height: 200
        Entity {
            ...
        }
    }

    Scene3DView {
        id: view2
        scene3D: mainScene3D
        width: 200
        height: 200
        x: 200
        Entity {
            ...
        }
    }
    \endqml

    There are a few limitations when using Scene3DView:
    \list
    \li The Scene3D compositingMode has to be set to FBO
    \li The Scene3D is sized to occupy the full window size (at the very least
    it must be sized as wide as the area occupied by all Scene3DViews)
    \li The Scene3D instance is instantiated prior to any Scene3DView
    \li The Scene3D entity property is left unset
    \endlist

    Scene3D behaves likes a texture atlas from which all Scene3DView instances.
    For this reason, care should be taken that only the first Scene3DView
    declared in the scene clears the color/depth. Additionally overlapping
    Scene3DView instances is discouraged as this might not produce the expected
    output.

    It is expected that a Scene3DView's Entity provide a RenderSettings with a
    valid SceneGraph. Please note that only the RenderSettings of the first
    Scene3DView instantiated will be taken into account.

    There are no restriction on the sharing of elements between different scenes
    in different Scene3DView instances.
 */

namespace {

Qt3DRender::QFrameGraphNode *frameGraphFromEntity(Qt3DCore::QEntity *entity)
{
    const auto renderSettingsComponents = entity->componentsOfType<Qt3DRender::QRenderSettings>();

    if (renderSettingsComponents.size() > 0) {
        Qt3DRender::QRenderSettings *renderSettings = renderSettingsComponents.first();
        return renderSettings->activeFrameGraph();
    }
    return nullptr;
}

}

Scene3DView::Scene3DView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_scene3D(nullptr)
    , m_entity(nullptr)
    , m_previousFGParent(nullptr)
    , m_holderEntity(new Qt3DCore::QEntity())
    , m_holderLayer(new Qt3DRender::QLayer())
    , m_holderLayerFilter(new Qt3DRender::QLayerFilter())
    , m_holderViewport(new Qt3DRender::QViewport())
    , m_dirtyFlags(DirtyNode|DirtyTexture)
    , m_texture(nullptr)
{
    setFlag(QQuickItem::ItemHasContents, true);\

    m_holderLayer->setRecursive(true);
    m_holderEntity->addComponent(m_holderLayer);
    m_holderLayerFilter->setParent(m_holderViewport);
    m_holderLayerFilter->addLayer(m_holderLayer);
}

Scene3DView::~Scene3DView()
{
    if (m_entity)
        abandonSubtree(m_entity);

    if (m_scene3D)
        m_scene3D->removeView(this);
}

Qt3DCore::QEntity *Scene3DView::entity() const
{
    return m_entity;
}

Scene3DItem *Scene3DView::scene3D() const
{
    return m_scene3D;
}

Qt3DCore::QEntity *Scene3DView::viewSubtree() const
{
    return m_holderEntity;
}

QFrameGraphNode *Scene3DView::viewFrameGraph() const
{
    return m_holderViewport;
}

// Called by Scene3DRender::beforeSynchronizing in RenderThread
void Scene3DView::setTexture(QSGTexture *texture)
{
    m_dirtyFlags |= DirtyTexture;
    m_texture = texture;
    QQuickItem::update();
}

QSGTexture *Scene3DView::texture() const
{
    return m_texture;
}

// Called by Scene3DRender::beforeSynchronizing in RenderThread
void Scene3DView::markSGNodeDirty()
{
    m_dirtyFlags |= DirtyNode;
    QQuickItem::update();
}

// Main Thread
void Scene3DView::setEntity(Qt3DCore::QEntity *entity)
{
    if (m_entity == entity)
        return;

    if (m_entity)
        abandonSubtree(m_entity);

    m_entity = entity;
    emit entityChanged();

    if (m_entity)
        adoptSubtree(m_entity);
}

// Main Thread
void Scene3DView::setScene3D(Scene3DItem *scene3D)
{
    if (m_scene3D == scene3D)
        return;

    if (m_scene3D) {
        m_scene3D->removeView(this);
        QObject::disconnect(m_scene3DDestroyedConnection);
    }

    setTexture(nullptr);
    m_scene3D = scene3D;
    emit scene3DChanged();


    if (m_scene3D) {
        m_scene3DDestroyedConnection = QObject::connect(m_scene3D,
                                                        &Scene3DItem::destroyed,
                                                        this,
                                                        [this] {
            m_scene3D = nullptr;
        });
        m_scene3D->addView(this);
    }
}

// Render Thread
QSGNode *Scene3DView::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    Scene3DSGNode *fboNode = static_cast<Scene3DSGNode *>(node);
    if (fboNode == nullptr)
        fboNode = new Scene3DSGNode();

    // We only need to draw a sub part of the texture based
    // on our size, Scene3D essentially acts as a TextureAtlas
    const QRectF itemRect(mapRectToScene(boundingRect()));
    const QSize winSize = window() ? window()->size() : QSize();
    const QRectF normalizedViewportRect(itemRect.x() / winSize.width(),
                                        itemRect.y() / winSize.height(),
                                        itemRect.width() / winSize.width(),
                                        itemRect.height() / winSize.height());
    // Swap Y axis to match GL coordinates
    const QRectF textureRect(itemRect.x() / winSize.width(),
                             1.0f - (itemRect.y() / winSize.height()),
                             itemRect.width() / winSize.width(),
                             -(itemRect.height() / winSize.height()));

    // TO DO: Should be done from main thread
    // updateViewport
    m_holderViewport->setNormalizedRect(normalizedViewportRect);

    // update node rect and texture coordinates
    fboNode->setRect(boundingRect(), textureRect);

    if (m_dirtyFlags & DirtyTexture) {
        fboNode->setTexture(m_texture);
        m_dirtyFlags.setFlag(DirtyTexture, false);
        // Show FBO Node at this point
        fboNode->show();
    }
    if (m_dirtyFlags & DirtyNode) {
        fboNode->markDirty(QSGNode::DirtyMaterial);
        m_dirtyFlags.setFlag(DirtyNode, false);
    }

    return fboNode;
}

// Main Thread
void Scene3DView::adoptSubtree(Qt3DCore::QEntity *subtree)
{
    // Reparent FrameGraph
    Qt3DRender::QFrameGraphNode *fgNode = frameGraphFromEntity(subtree);
    if (fgNode) {
        m_previousFGParent = fgNode->parentNode();
        fgNode->setParent(m_holderLayerFilter);
    }

    // Insert Entity Subtree
    subtree->setParent(m_holderEntity);
}

// Main Thread
void Scene3DView::abandonSubtree(Qt3DCore::QEntity *subtree)
{
    // Remove FrameGraph part
    Qt3DRender::QFrameGraphNode *fgNode = frameGraphFromEntity(subtree);
    if (fgNode)
        fgNode->setParent(m_previousFGParent);

    // Remove Entity Subtree
    subtree->setParent(Q_NODE_NULLPTR);
}

} // Qt3DRender

QT_END_NAMESPACE
