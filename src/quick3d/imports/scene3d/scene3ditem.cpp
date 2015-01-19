/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "scene3ditem.h"

#include <Qt3DCore/QAspectEngine>
#include <Qt3DRenderer/QRenderAspect>

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

#include <QSurface>

#include <QSGSimpleTextureNode>

QT_BEGIN_NAMESPACE

class ContextSaver
{
public:
    explicit ContextSaver(QOpenGLContext *context = QOpenGLContext::currentContext())
        : m_context(context),
          m_surface(context ? context->surface() : Q_NULLPTR)
    {
    }

    ~ContextSaver()
    {
        if (m_context)
            m_context->makeCurrent(m_surface);
    }

    QOpenGLContext *context() const { return m_context; }
    QSurface *surface() const { return m_surface; }

private:
    QOpenGLContext * const m_context;
    QSurface * const m_surface;
};

class FrameBufferObjectRenderer : public QQuickFramebufferObject::Renderer
{
public:
    FrameBufferObjectRenderer(const Scene3DItem *item,
                              Qt3D::QAspectEngine *aspectEngine,
                              Qt3D::QRenderAspect *renderAspect)
        : m_item(item),
          m_aspectEngine(aspectEngine),
          m_renderAspect(renderAspect)
    {
        ContextSaver saver;

        QVariantMap data;
        data.insert(QStringLiteral("surface"), QVariant::fromValue(saver.surface()));
        m_aspectEngine->setData(data);

        m_renderAspect->renderInitialize(saver.context());
        scheduleRootEntityChange();
    }

    void render() Q_DECL_OVERRIDE
    {
        if (m_aspectEngine->rootEntity() != m_item->entity())
            scheduleRootEntityChange();

        ContextSaver saver;
        Q_UNUSED(saver)

        m_renderAspect->renderSynchronous();
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    void scheduleRootEntityChange()
    {
        Scene3DItem *item = const_cast<Scene3DItem*>(m_item);
        QMetaObject::invokeMethod(item, "applyRootEntityChange", Qt::QueuedConnection);
    }

    const Scene3DItem *m_item;
    Qt3D::QAspectEngine *m_aspectEngine;
    Qt3D::QRenderAspect *m_renderAspect;
};

Scene3DItem::Scene3DItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent),
      m_entity(Q_NULLPTR),
      m_aspectEngine(new Qt3D::QAspectEngine(this)),
      m_renderAspect(new Qt3D::QRenderAspect(Qt3D::QRenderAspect::Synchronous))
{
    setFlag(QQuickItem::ItemHasContents, true);
    m_aspectEngine->registerAspect(m_renderAspect);
    m_aspectEngine->initialize();
}

Qt3D::QEntity *Scene3DItem::entity() const
{
    return m_entity;
}

void Scene3DItem::setEntity(Qt3D::QEntity *entity)
{
    if (entity == m_entity)
        return;

    m_entity = entity;
    emit entityChanged();
}

void Scene3DItem::applyRootEntityChange()
{
    if (m_aspectEngine->rootEntity() != m_entity)
        m_aspectEngine->setRootEntity(m_entity);
}

QQuickFramebufferObject::Renderer *Scene3DItem::createRenderer() const
{
    return new FrameBufferObjectRenderer(this, m_aspectEngine, m_renderAspect);
}

QSGNode *Scene3DItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData)
{
    if (!node) {
        node = QQuickFramebufferObject::updatePaintNode(node, nodeData);
        QSGSimpleTextureNode *textureNode = static_cast<QSGSimpleTextureNode *>(node);
        if (textureNode)
            textureNode->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
        return node;
    }
    return QQuickFramebufferObject::updatePaintNode(node, nodeData);
}

QT_END_NAMESPACE
