/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "scene3ditem.h"

#include <Qt3DCore/QAspectEngine>
#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DInput/QInputAspect>

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QSurface>
#include <QQuickWindow>

#include <QSGTexture>
#include <QSGMaterial>
#include <QSGNode>
#include <QOpenGLFunctions>

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

class Scene3DSGNode;

/*!
    \class Qt3D::Scene3DRenderer
    \internal

    \brief The Qt3D::Scene3DRenderer class takes care of rendering a Qt3D scene
    within a Framebuffer object to be used by the QtQuick 2 renderer.

    The Qt3D::Scene3DRenderer class renders a Qt3D scene as provided by a Qt3D::Scene3DItem
 */
class Scene3DRenderer : public QObject
{
    Q_OBJECT
public:
    Scene3DRenderer(Scene3DItem *item,
                    Qt3D::QAspectEngine *aspectEngine,
                    Qt3D::QRenderAspect *renderAspect)
        : QObject()
        , m_item(item)
        , m_aspectEngine(aspectEngine)
        , m_renderAspect(renderAspect)
        , m_multisampledFBO(Q_NULLPTR)
        , m_finalFBO(Q_NULLPTR)
        , m_texture(Q_NULLPTR)
    {
        Q_CHECK_PTR(m_item);
        QObject::connect(m_item->window(), SIGNAL(beforeRendering()), this, SLOT(render()));
        ContextSaver saver;

        QVariantMap data;
        data.insert(QStringLiteral("surface"), QVariant::fromValue(saver.surface()));
        data.insert(QStringLiteral("eventSource"), QVariant::fromValue(m_item));
        m_aspectEngine->setData(data);

        m_renderAspect->renderInitialize(saver.context());
        scheduleRootEntityChange();
    }

    ~Scene3DRenderer()
    {
        delete m_multisampledFBO;
        delete m_finalFBO;
        delete m_texture;
    }

    QOpenGLFramebufferObject *createMultisampledFramebufferObject(const QSize &size)
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size)
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Depth);
        return new QOpenGLFramebufferObject(size, format);
    }

    void scheduleRootEntityChange()
    {
        QMetaObject::invokeMethod(m_item, "applyRootEntityChange", Qt::QueuedConnection);
    }

    void setSGNode(Scene3DSGNode *node) Q_DECL_NOEXCEPT;

public Q_SLOTS:
    void render();

private:
    Scene3DItem *m_item;
    Qt3D::QAspectEngine *m_aspectEngine;
    Qt3D::QRenderAspect *m_renderAspect;
    QOpenGLFramebufferObject *m_multisampledFBO;
    QOpenGLFramebufferObject *m_finalFBO;
    QSGTexture *m_texture;
    Scene3DSGNode *m_node;
};

/*!
    \class Qt3D::SCene3DMaterialShader
    \internal

    \brief The Qt3D::Scene3DSGMaterialShader class is a custom
    QSGMaterialShader subclass instantiated by a Qt3D::Scene3DSGMateria1

    The Qt3D::Scene3DSGMaterialShader provides a shader that renders a texture
    using premultiplied alpha.
 */
class Scene3DSGMaterialShader : public QSGMaterialShader
{
public:
    void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) Q_DECL_FINAL;

    const char * const *attributeNames() const Q_DECL_FINAL
    {
        static char const *const attr[] = { "qt_VertexPosition", "qt_VertexTexCoord", 0 };
        return attr;
    }

    static QSGMaterialType type;

protected:
    const char *vertexShader() const Q_DECL_FINAL
    {
        return ""
               "uniform highp mat4 qt_Matrix;                       \n"
               "attribute highp vec4 qt_VertexPosition;             \n"
               "attribute highp vec2 qt_VertexTexCoord;             \n"
               "varying highp vec2 qt_TexCoord;                     \n"
               "void main() {                                       \n"
               "   qt_TexCoord = qt_VertexTexCoord;                 \n"
               "   gl_Position = qt_Matrix * qt_VertexPosition;     \n"
               "}";
    }

    const char *fragmentShader() const Q_DECL_FINAL
    {
        return ""
               "uniform highp sampler2D source;                         \n"
               "uniform highp float qt_Opacity;                         \n"
               "varying highp vec2 qt_TexCoord;                         \n"
               "void main() {                                           \n"
               "   highp vec4 p = texture2D(source, qt_TexCoord);       \n"
               "   gl_FragColor = vec4(p.rgb * p.a, p.a) * qt_Opacity;  \n"
               "}";
    }

    void initialize() Q_DECL_FINAL
    {
        m_matrixId = program()->uniformLocation("qt_Matrix");
        m_opacityId = program()->uniformLocation("qt_Opacity");
    }

private:
    int m_matrixId;
    int m_opacityId;
};

QSGMaterialType Scene3DSGMaterialShader::type;

/*!
    \class Qt3D::Scene3DSGMaterial
    \internal
    \inherit QSGMaterial

    \brief The Qt3D::Scene3DSGMaterial class is a custom QSGMaterial subclass used to
    render a Qt3D::Scene3DSGNode

    The Qt3D::Scene3DSGMaterial class renders a texture using premultiplied
    alpha unlike the QSGSimpleTextureMaterial.

    This is needed to properly integrate alpha blending from a Qt3D scene
    within a QtQuick 2 scene.
 */
class Scene3DSGMaterial : public QSGMaterial
{
public:
    Scene3DSGMaterial()
        : QSGMaterial()
        , m_texture(Q_NULLPTR)
    {}

    void setTexture(QSGTexture *texture) Q_DECL_NOEXCEPT
    {
        m_texture = texture;
        setFlag(Blending, m_texture ? m_texture->hasAlphaChannel() : false);
    }

    QSGTexture *texture() const Q_DECL_NOEXCEPT { return m_texture; }
    QSGMaterialType *type() const Q_DECL_FINAL { return &Scene3DSGMaterialShader::type; }
    QSGMaterialShader *createShader() const Q_DECL_FINAL { return new Scene3DSGMaterialShader(); }

private:
    QSGTexture *m_texture;
};

/*!
    \class Qt3D::Scene3DSGNode
    \internal

    \brief The Qt3D::Scene3DSGNode class is a simple QSGeometryNode subclass that
    uses a Qt3D::Scene3DMaterial

    The Qt3D::Scene3DSGNode allows to render a simple rect texture with
    premultiplied alpha.
 */
class Scene3DSGNode : public QSGGeometryNode
{
public:
    Scene3DSGNode()
        : QSGGeometryNode()
        , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
    {
        setMaterial(&m_material);
        setOpaqueMaterial(&m_opaqueMaterial);
        setGeometry(&m_geometry);
    }

    void setTexture(QSGTexture *texture) Q_DECL_NOEXCEPT
    {
        m_material.setTexture(texture);
        m_opaqueMaterial.setTexture(texture);
        markDirty(DirtyMaterial);
    }

    QSGTexture *texture() const Q_DECL_NOEXCEPT { return m_material.texture(); }

    void setRect(const QRectF &rect)
    {
        if (rect != m_rect) {
            m_rect = rect;
            // Map the item's bounding rect to normalized texture coordinates
            const QRectF sourceRect(0.0f, 1.0f, 1.0f, -1.0f);
            QSGGeometry::updateTexturedRectGeometry(&m_geometry, m_rect, sourceRect);
            markDirty(DirtyGeometry);
        }
    }

    QRectF rect() const Q_DECL_NOEXCEPT { return m_rect; }

private:
    Scene3DSGMaterial m_material;
    Scene3DSGMaterial m_opaqueMaterial;
    QSGGeometry m_geometry;
    QRectF m_rect;
};


/*!
    \class Qt3D::Scene3DItem
    \internal

    \brief The Qt3D::Scene3DItem class is a QQuickItem subclass used to integrate
    a Qt3D scene into a QtQuick 2 scene.

    The Qt3D::Scene3DItem class renders a Qt3D scene, provided by a Qt3D::QEntity
    into a multisampled Framebuffer object that is later blitted into a non
    multisampled Framebuffer object to be then renderer through the use of a
    Qt3D::Scene3DSGNode with premultiplied alpha.
 */

Scene3DItem::Scene3DItem(QQuickItem *parent)
    : QQuickItem(parent),
      m_entity(Q_NULLPTR),
      m_aspectEngine(new Qt3D::QAspectEngine(this)),
      m_renderAspect(new Qt3D::QRenderAspect(Qt3D::QRenderAspect::Synchronous)),
      m_renderer(Q_NULLPTR)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::MouseButtonMask);
    setAcceptHoverEvents(true);

    m_aspectEngine->registerAspect(m_renderAspect);
    m_aspectEngine->initialize();
}

Scene3DItem::~Scene3DItem()
{
    m_renderAspect->renderShutdown();
}

QStringList Scene3DItem::aspects() const
{
    return m_aspects;
}

Qt3D::QEntity *Scene3DItem::entity() const
{
    return m_entity;
}

void Scene3DItem::setAspects(const QStringList &aspects)
{
    if (!m_aspects.isEmpty()) {
        qWarning() << "Aspects already set on the Scene3D, ignoring";
        return;
    }

    m_aspects = aspects;

    Q_FOREACH (const QString &aspect, m_aspects) {
        if (aspect == QStringLiteral("render")) // This one is hardwired anyway
            continue;
        if (aspect == QStringLiteral("input"))  {
            m_aspectEngine->registerAspect(new Qt3D::QInputAspect);
            continue;
        }

        m_aspectEngine->registerAspect(aspect);
    }

    emit aspectsChanged();
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

QSGNode *Scene3DItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    // If the node already exists
    // we delete it and recreate it
    // as we need to resize the FBO
    if (node) {
        delete node;
        node = Q_NULLPTR;
    }

    if (!m_renderer) {
        m_renderer = new Scene3DRenderer(this, m_aspectEngine, m_renderAspect);
    }

    Scene3DSGNode *fboNode = new Scene3DSGNode();
    fboNode->setRect(boundingRect());
    m_renderer->setSGNode(fboNode);
    return fboNode;
}

void Scene3DRenderer::setSGNode(Scene3DSGNode *node) Q_DECL_NOEXCEPT
{
    m_node = node;
    if (m_texture)
        node->setTexture(m_texture);
}

void Scene3DRenderer::render()
{
    if (!m_item || !m_item->window())
        return ;

    if (m_aspectEngine->rootEntity() != m_item->entity())
        scheduleRootEntityChange();

    ContextSaver saver;

    if (!m_multisampledFBO)
        m_multisampledFBO = createMultisampledFramebufferObject(m_item->boundingRect().size().toSize());

    if (!m_finalFBO) {
        m_finalFBO = createFramebufferObject(m_item->boundingRect().size().toSize());
        m_texture = m_item->window()->createTextureFromId(m_finalFBO->texture(), m_finalFBO->size());
        m_node->setTexture(m_texture);
    }

    // Bind FBO
    m_multisampledFBO->bind();
    // Render Qt3D Scene
    m_renderAspect->renderSynchronous();

    // We may have called doneCurrent() so restore the context.
    saver.context()->makeCurrent(saver.surface());

    // Blit multisampled FBO with non multisampled FBO with texture attachment
    QOpenGLFramebufferObject::blitFramebuffer(m_finalFBO, m_multisampledFBO);

    // Restore QtQuick FBO
    m_multisampledFBO->bindDefault();

    // Reset the state used by the Qt Quick scenegraph to avoid any
    // interference when rendering the rest of the UI.
    m_item->window()->resetOpenGLState();

    // Mark material as dirty to request a new frame
    m_node->markDirty(QSGNode::DirtyMaterial);

    // Request next frame
    m_item->window()->update();
}

inline static bool isPowerOfTwo(int x)
{
    // Assumption: x >= 1
    return x == (x & -x);
}

void Scene3DSGMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    Scene3DSGMaterial *tx = static_cast<Scene3DSGMaterial *>(newEffect);
    Scene3DSGMaterial *oldTx = static_cast<Scene3DSGMaterial *>(oldEffect);

    QSGTexture *t = tx->texture();

    bool npotSupported = const_cast<QOpenGLContext *>(state.context())
            ->functions()->hasOpenGLFeature(QOpenGLFunctions::NPOTTextureRepeat);
    if (!npotSupported) {
        QSize size = t->textureSize();
        const bool isNpot = !isPowerOfTwo(size.width()) || !isPowerOfTwo(size.height());
        if (isNpot) {
            t->setHorizontalWrapMode(QSGTexture::ClampToEdge);
            t->setVerticalWrapMode(QSGTexture::ClampToEdge);
        }
    }

    if (oldTx == 0 || oldTx->texture()->textureId() != t->textureId())
        t->bind();
    else
        t->updateBindOptions();

    if (state.isMatrixDirty())
        program()->setUniformValue(m_matrixId, state.combinedMatrix());

    if (state.isOpacityDirty())
        program()->setUniformValue(m_opacityId, state.opacity());
}


QT_END_NAMESPACE

#include "scene3ditem.moc"
