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

#include "scene3ditem_p.h"
#include "scene3dlogging_p.h"

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
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

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

    The Qt3D::Scene3DRenderer class renders a Qt3D scene as provided by a Qt3D::Scene3DItem.
    It owns the aspectEngine even though it doesn't instantiate it.

    The shutdown procedure is a two steps process that goes as follow:

    \li The window is closed

    \li This triggers the windowsChanged signal which the Scene3DRenderer
    uses to perform the necessary cleanups in the QSGRenderThread (destroys
    DebugLogger ...) with the shutdown slot (queued connection).

    \li The destroyed signal of the window is also connected to the
    Scene3DRenderer. When triggered in the context of the main thread, the
    cleanup slot is called.

    There is an alternate shutdown procedure in case the QQuickItem is
    destroyed with an active window which can happen in the case where the
    Scene3D is used with a QtQuick Loader

    In that case the shutdown procedure goes the same except that the destroyed
    signal of the window is not called. Therefore the cleanup method is invoked
    to properly destroy the aspect engine.
 */

// Lives in the main thread
// Used to delete the Qt3D aspect engine in the main thread
class Scene3DCleaner : public QObject
{
    Q_OBJECT
public:
    Scene3DCleaner()
        : QObject()
        , m_renderer(Q_NULLPTR)
    {}

    ~Scene3DCleaner()
    {
        qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
    }

    void setRenderer(Scene3DRenderer *renderer)
    {
        m_renderer = renderer;
    }

public Q_SLOTS:
    void cleanup();

private:
    Scene3DRenderer *m_renderer;
};

// Lives in the aspect thread (QSGRenderThread)
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
        , m_multisample(false) // this value is not used, will be synced from the Scene3DItem instead
        , m_lastMultisample(false)
    {
        Q_CHECK_PTR(m_item);
        Q_CHECK_PTR(m_item->window());

        QObject::connect(m_item->window(), &QQuickWindow::beforeRendering, this, &Scene3DRenderer::render, Qt::DirectConnection);
        QObject::connect(m_item, &QQuickItem::windowChanged, this, &Scene3DRenderer::onWindowChangedQueued, Qt::QueuedConnection);

        ContextSaver saver;

        QVariantMap data;
        data.insert(QStringLiteral("surface"), QVariant::fromValue(saver.surface()));
        data.insert(QStringLiteral("eventSource"), QVariant::fromValue(m_item));
        m_aspectEngine->setData(data);

        m_renderAspect->renderInitialize(saver.context());
        scheduleRootEntityChange();
    }

    // The Scene3DRender is delete by itself with the cleanup slot
    ~Scene3DRenderer()
    {
        qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
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

    void setCleanerHelper(Scene3DCleaner *cleaner)
    {
        m_cleaner = cleaner;
        if (m_cleaner) {
            // Window closed case
            QObject::connect(m_item->window(), &QQuickWindow::destroyed, m_cleaner, &Scene3DCleaner::cleanup);
            m_cleaner->setRenderer(this);
        }
    }

    void synchronize();

public Q_SLOTS:
    void render();

    // Executed in the QtQuick render thread.
    void shutdown()
    {
        qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();

        // Set to null so that subsequent calls to render
        // would return early
        m_item = Q_NULLPTR;

        // Shutdown the Renderer Aspect while the OpenGL context
        // is still valid
        m_renderAspect->renderShutdown();
    }

    // SGThread
    void onWindowChangedQueued(QQuickWindow *w)
    {
        if (w == Q_NULLPTR) {
            qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
            shutdown();
            // Will only trigger something with the Loader case
            // The window closed cases is handled by the window's destroyed
            // signal
            QMetaObject::invokeMethod(m_cleaner, "cleanup");
        }
    }

private:
    Scene3DItem *m_item; // Will be released by the QQuickWindow/QML Engine
    Qt3D::QAspectEngine *m_aspectEngine; // Will be released by the Scene3DRendererCleaner
    Qt3D::QRenderAspect *m_renderAspect; // Will be released by the aspectEngine
    QScopedPointer<QOpenGLFramebufferObject> m_multisampledFBO;
    QScopedPointer<QOpenGLFramebufferObject> m_finalFBO;
    QScopedPointer<QSGTexture> m_texture;
    Scene3DSGNode *m_node; // Will be released by the QtQuick SceneGraph
    Scene3DCleaner *m_cleaner;
    QSize m_lastSize;
    bool m_multisample;
    bool m_lastMultisample;

    friend class Scene3DCleaner;
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
        QOpenGLContext *ctx = QOpenGLContext::currentContext();
        if (ctx->format().version() >= qMakePair(3, 2) && ctx->format().profile() == QSurfaceFormat::CoreProfile) {
            return ""
                   "#version 150 core                                   \n"
                   "uniform mat4 qt_Matrix;                             \n"
                   "in vec4 qt_VertexPosition;                          \n"
                   "in vec2 qt_VertexTexCoord;                          \n"
                   "out vec2 qt_TexCoord;                               \n"
                   "void main() {                                       \n"
                   "   qt_TexCoord = qt_VertexTexCoord;                 \n"
                   "   gl_Position = qt_Matrix * qt_VertexPosition;     \n"
                   "}";
        } else {
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
    }

    const char *fragmentShader() const Q_DECL_FINAL
    {
        QOpenGLContext *ctx = QOpenGLContext::currentContext();
        if (ctx->format().version() >= qMakePair(3, 2) && ctx->format().profile() == QSurfaceFormat::CoreProfile) {
            return ""
                   "#version 150 core                                   \n"
                   "uniform sampler2D source;                           \n"
                   "uniform float qt_Opacity;                           \n"
                   "in vec2 qt_TexCoord;                                \n"
                   "out vec4 fragColor;                                 \n"
                   "void main() {                                       \n"
                   "   vec4 p = texture(source, qt_TexCoord);         \n"
                   "   fragColor = vec4(p.rgb * p.a, qt_Opacity * p.a); \n"
                   "}";
        } else {
            return ""
                   "uniform highp sampler2D source;                         \n"
                   "uniform highp float qt_Opacity;                         \n"
                   "varying highp vec2 qt_TexCoord;                         \n"
                   "void main() {                                           \n"
                   "   highp vec4 p = texture2D(source, qt_TexCoord);       \n"
                   "   gl_FragColor = vec4(p.rgb * p.a, qt_Opacity * p.a);  \n"
                   "}";
        }
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
        qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
    }

    ~Scene3DSGNode()
    {
        qCDebug(Scene3D) << Q_FUNC_INFO << QThread::currentThread();
        // The Scene3DSGNode is deleted by the QSGRenderThread when the SceneGraph
        // is terminated.
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
    : QQuickItem(parent)
    , m_entity(Q_NULLPTR)
    , m_aspectEngine(new Qt3D::QAspectEngine())
    , m_renderAspect(new Qt3D::QRenderAspect(Qt3D::QRenderAspect::Synchronous))
    , m_renderer(Q_NULLPTR)
    , m_rendererCleaner(new Scene3DCleaner())
    , m_multisample(true)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::MouseButtonMask);
    setAcceptHoverEvents(true);

    m_aspectEngine->registerAspect(m_renderAspect);
    m_aspectEngine->initialize();
}

Scene3DItem::~Scene3DItem()
{
    // When the window is closed, it first destroys all of its children. At
    // this point, Scene3DItem is destroyed but the Renderer, AspectEngine and
    // Scene3DSGNode still exist and will perform their cleanup on their own.
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

    // Aspects are owned by the aspect engine
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

/*!
    \return \c true if a multisample renderbuffer is in use.
 */
bool Scene3DItem::multisample() const
{
    return m_multisample;
}

/*!
    Enables or disables the usage of multisample renderbuffers based on \a enable.

    By default multisampling is enabled. If the OpenGL implementation has no
    support for multisample renderbuffers or framebuffer blits, the request to
    use multisampling is ignored.

    \note Refrain from changing the value frequently as it involves expensive
    and potentially slow initialization of framebuffers and other OpenGL
    resources.
 */
void Scene3DItem::setMultisample(bool enable)
{
    if (m_multisample != enable) {
        m_multisample = enable;
        emit multisampleChanged();
        update();
    }
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

    if (m_renderer == Q_NULLPTR) {
        m_renderer = new Scene3DRenderer(this, m_aspectEngine, m_renderAspect);
        m_renderer->setCleanerHelper(m_rendererCleaner);
    }

    // The main thread is blocked, it is now time to sync data between the renderer and the item.
    m_renderer->synchronize();

    Scene3DSGNode *fboNode = new Scene3DSGNode();
    fboNode->setRect(boundingRect());
    m_renderer->setSGNode(fboNode);
    return fboNode;
}

void Scene3DRenderer::synchronize()
{
    m_multisample = m_item->multisample();
}

void Scene3DRenderer::setSGNode(Scene3DSGNode *node) Q_DECL_NOEXCEPT
{
    m_node = node;
    if (!m_texture.isNull())
        node->setTexture(m_texture.data());
}

void Scene3DRenderer::render()
{
    if (!m_item || !m_item->window())
        return;

    QQuickWindow *window = m_item->window();

    if (m_aspectEngine->rootEntity() != m_item->entity())
        scheduleRootEntityChange();

    ContextSaver saver;

    const QSize currentSize = m_item->boundingRect().size().toSize() * window->effectiveDevicePixelRatio();
    const bool forceRecreate = currentSize != m_lastSize || m_multisample != m_lastMultisample;

    // Rebuild FBO and textures if never created or a resize has occurred
    if ((m_multisampledFBO.isNull() || forceRecreate) && m_multisample) {
        qCDebug(Scene3D) << Q_FUNC_INFO << "Creating multisample framebuffer";
        m_multisampledFBO.reset(createMultisampledFramebufferObject(currentSize));
        if (m_multisampledFBO->format().samples() == 0 || !QOpenGLFramebufferObject::hasOpenGLFramebufferBlit()) {
            qCDebug(Scene3D) << Q_FUNC_INFO << "Failed to create multisample framebuffer";
            m_multisample = false;
            m_multisampledFBO.reset(Q_NULLPTR);
        }
    }

    if (m_finalFBO.isNull() || forceRecreate) {
        m_finalFBO.reset(createFramebufferObject(currentSize));
        m_texture.reset(window->createTextureFromId(m_finalFBO->texture(), m_finalFBO->size(), QQuickWindow::TextureHasAlphaChannel));
        m_node->setTexture(m_texture.data());
    }

    // Store the current size as a comparison
    // point for the next frame
    m_lastSize = currentSize;
    m_lastMultisample = m_multisample;

    //Only try to use MSAA when available
    if (m_multisample) {
        // Bind FBO
        m_multisampledFBO->bind();

        // Render Qt3D Scene
        m_renderAspect->renderSynchronous();

        // We may have called doneCurrent() so restore the context.
        if (saver.context()->surface() != saver.surface())
            saver.context()->makeCurrent(saver.surface());

        // Blit multisampled FBO with non multisampled FBO with texture attachment
        QOpenGLFramebufferObject::blitFramebuffer(m_finalFBO.data(), m_multisampledFBO.data());

        // Restore QtQuick FBO
        m_multisampledFBO->bindDefault();
    } else {
        // Bind FBO
        m_finalFBO->bind();

        // Render Qt3D Scene
        m_renderAspect->renderSynchronous();

        // We may have called doneCurrent() so restore the context.
        if (saver.context()->surface() != saver.surface())
            saver.context()->makeCurrent(saver.surface());

        // Restore QtQuick FBO
        m_finalFBO->bindDefault();
    }

    // Reset the state used by the Qt Quick scenegraph to avoid any
    // interference when rendering the rest of the UI.
    window->resetOpenGLState();

    // Mark material as dirty to request a new frame
    m_node->markDirty(QSGNode::DirtyMaterial);

    // Request next frame
    window->update();
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

void Scene3DCleaner::cleanup()
{
    Q_ASSERT(m_renderer);
    delete m_renderer->m_aspectEngine;
    m_renderer->m_aspectEngine = Q_NULLPTR;
    m_renderer->deleteLater();
    deleteLater();
}

} // Qt3D

QT_END_NAMESPACE

#include "scene3ditem.moc"
