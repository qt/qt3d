/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglpainter.h"
#include "qglpainter_p.h"
#include "qglabstracteffect.h"
#include "qglext_p.h"
#include <QtOpenGL/qglpixelbuffer.h>
#include <QtOpenGL/qglshaderprogram.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qmap.h>
#if !defined(QT_NO_THREAD)
#include <QtCore/qthreadstorage.h>
#endif
#include "qglflatcoloreffect_p.h"
#include "qglflattextureeffect_p.h"
#include "qgllitmaterialeffect_p.h"
#include "qgllittextureeffect_p.h"
#include "qglpickcolors_p.h"
#include "qgltexture2d.h"
#include "qgltexturecube.h"
#include "qgeometrydata.h"
#include "qglvertexbundle_p.h"
#include "qmatrix4x4stack_p.h"
#include "qglwidgetsurface.h"
#include "qglpixelbuffersurface.h"
#include "qglpaintersurface_p.h"

#undef glActiveTexture

QT_BEGIN_NAMESPACE

/*!
    \class QGLPainter
    \brief The QGLPainter class provides portable API's for rendering into a GL context.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::painting

    TBD - lots of TBD

    All QGLPainter instances on a context share the same context state:
    matrices, effects, vertex attributes, etc.  For example, calling
    ortho() on one QGLPainter instance for a context will alter the
    projectionMatrix() as seen by the other QGLPainter instances.
*/

/*!
    \enum QGLPainter::Update
    This enum defines the values that were changed since the last QGLPainter::update().

    \value UpdateColor The color has been updated.
    \value UpdateModelViewMatrix The modelview matrix has been updated.
    \value UpdateProjectionMatrix The projection matrix has been updated.
    \value UpdateMatrices The combination of UpdateModelViewMatrix and
           UpdateProjectionMatrix.
    \value UpdateLights The lights have been updated.
    \value UpdateMaterials The material parameters have been updated.
    \value UpdateViewport The viewport needs to be updated because the
           drawing surface has changed.
    \value UpdateAll All values have been updated.  This is specified
           when an effect is activated.
*/

#define QGLPAINTER_CHECK_PRIVATE() \
    Q_ASSERT_X(d, "QGLPainter", "begin() has not been called or it failed")

QGLPainterPrivate::QGLPainterPrivate()
    : ref(1),
      eye(QGL::NoEye),
      lightModel(0),
      defaultLightModel(0),
      defaultLight(0),
      frontMaterial(0),
      backMaterial(0),
      defaultMaterial(0),
      frontColorMaterial(0),
      backColorMaterial(0),
      viewingCube(QVector3D(-1, -1, -1), QVector3D(1, 1, 1)),
      color(255, 255, 255, 255),
      updates(QGLPainter::UpdateAll),
      pick(0),
      boundVertexBuffer(0),
      boundIndexBuffer(0),
      renderSequencer(0),
      isFixedFunction(true) // Updated by QGLPainter::begin()
{
    context = 0;
    effect = 0;
    userEffect = 0;
    standardEffect = QGL::FlatColor;
    memset(stdeffects, 0, sizeof(stdeffects));
}

QGLPainterPrivate::~QGLPainterPrivate()
{
    delete defaultLightModel;
    delete defaultLight;
    delete defaultMaterial;
    delete frontColorMaterial;
    delete backColorMaterial;
    for (int effect = 0; effect < QGL_MAX_STD_EFFECTS; ++effect)
        delete stdeffects[effect];
    delete pick;
    qDeleteAll(cachedPrograms);
    delete renderSequencer;
}

QGLPainterPickPrivate::QGLPainterPickPrivate()
{
    isPicking = false;
    objectPickId = -1;
    pickColorIndex = -1;
    pickColor = 0;
    defaultPickEffect = new QGLFlatColorEffect();
}

QGLPainterPickPrivate::~QGLPainterPickPrivate()
{
    delete defaultPickEffect;
}

#if !defined(QT_NO_THREAD)

// QGLContext's are thread-specific, so QGLPainterPrivateCache should be too.

typedef QThreadStorage<QGLPainterPrivateCache *> QGLPainterPrivateStorage;
Q_GLOBAL_STATIC(QGLPainterPrivateStorage, painterPrivateStorage)
static QGLPainterPrivateCache *painterPrivateCache()
{
    QGLPainterPrivateCache *cache = painterPrivateStorage()->localData();
    if (!cache) {
        cache = new QGLPainterPrivateCache();
        painterPrivateStorage()->setLocalData(cache);
    }
    return cache;
}

#else

Q_GLOBAL_STATIC(QGLPainterPrivateCache, painterPrivateCache)

#endif

QGLPainterPrivateCache::QGLPainterPrivateCache()
{
    connect(QGLSignalProxy::instance(),
            SIGNAL(aboutToDestroyContext(const QGLContext *)),
            this,
            SLOT(contextDestroyed(const QGLContext *)));
}

QGLPainterPrivateCache::~QGLPainterPrivateCache()
{
}

QGLPainterPrivate *QGLPainterPrivateCache::fromContext
    (const QGLContext *context)
{
    QGLPainterPrivate *priv = cache.value(context, 0);
    if (priv)
        return priv;
    priv = new QGLPainterPrivate();
    priv->context = context;
    cache.insert(context, priv);
    return priv;
}

QGLPainterPrivateCache *QGLPainterPrivateCache::instance()
{
    return painterPrivateCache();
}

void QGLPainterPrivateCache::contextDestroyed(const QGLContext *context)
{
    QGLPainterPrivate *priv = cache.value(context, 0);
    if (priv) {
        priv->context = 0;
        cache.remove(context);
        if (!priv->ref.deref())
            delete priv;
    }
    emit destroyedContext(context);
}

/*!
    Constructs a new GL painter.  Call begin() to attach the
    painter to a GL context.

    \sa begin()
*/
QGLPainter::QGLPainter()
    : d_ptr(0)
{
}

/*!
    Constructs a new GL painter and attaches it to \a context.
    It is not necessary to call begin() after construction.

    \sa begin()
*/
QGLPainter::QGLPainter(const QGLContext *context)
    : d_ptr(0)
{
    begin(context);
}

/*!
    Constructs a new GL painter and attaches it to the GL
    context associated with \a widget.  It is not necessary to
    call begin() after construction.

    \sa begin(), isActive()
*/
QGLPainter::QGLPainter(QGLWidget *widget)
    : d_ptr(0)
{
    begin(widget);
}

/*!
    Constructs a new GL painter and attaches it to the GL context associated
    with \a painter.  It is assumed that \a painter is the currently
    active painter and that it is associated with the current GL context.

    If \a painter is not using an OpenGL paint engine, then isActive()
    will return false; true otherwise.

    This constructor is typically used when mixing regular Qt painting
    operations and GL painting operations on a widget that is being
    drawn using the OpenGL graphics system.

    \sa begin(), isActive()
*/
QGLPainter::QGLPainter(QPainter *painter)
    : d_ptr(0)
{
    begin(painter);
}

/*!
    Constructs a new GL painter and attaches it to the GL context associated
    with \a surface.

    \sa begin(), isActive()
*/
QGLPainter::QGLPainter(QGLAbstractSurface *surface)
    : d_ptr(0)
{
    begin(surface);
}

/*!
    Destroys this GL painter.
*/
QGLPainter::~QGLPainter()
{
    end();
}

/*!
    Begins painting on QGLContext::currentContext().  Returns false
    if there is no GL context current.

    \sa end()
*/
bool QGLPainter::begin()
{
    return begin(QGLContext::currentContext());
}

/*!
    Begins painting on \a context.  If painting was already in progress,
    then this function will call end() first.  The \a context will be
    made current if it is not already current.

    Returns true if painting can begin; false otherwise.

    All QGLPainter instances on a context share the same context state:
    matrices, the effect(), vertex attributes, etc.  For example,
    calling ortho() on one QGLPainter instance for a context will
    alter the projectionMatrix() as seen by the other QGLPainter instances.

    \sa end(), isActive()
*/
bool QGLPainter::begin(const QGLContext *context)
{
    if (!context)
        return false;
    end();
    return begin(context, QGLAbstractSurface::createSurfaceForContext(context));
}

/*!
    \internal
*/
bool QGLPainter::begin
    (const QGLContext *context, QGLAbstractSurface *surface,
     bool destroySurface)
{
    // If we don't have a context specified, then use the one
    // that the surface just made current.
    if (!context)
        context = QGLContext::currentContext();

    // Find the QGLPainterPrivate for the context, or create a new one.
    d_ptr = painterPrivateCache()->fromContext(context);
    d_ptr->ref.ref();
    if (d_ptr->renderSequencer)
    {
        d_ptr->renderSequencer->reset();
        d_ptr->renderSequencer->setPainter(this);
    }

    // Activate the main surface for the context.
    QGLAbstractSurface *prevSurface;
    if (d_ptr->surfaceStack.isEmpty()) {
        prevSurface = 0;
    } else {
        // We are starting a nested begin()/end() scope, so switch
        // to the new main surface rather than activate from scratch.
        prevSurface = d_ptr->surfaceStack.last().surface;
        prevSurface->deactivate(surface);
    }
    if (!surface->activate(prevSurface)) {
        if (prevSurface)
            prevSurface->activate(surface);
        if (destroySurface)
            delete surface;
        if (!d_ptr->ref.deref())
            delete d_ptr;
        d_ptr = 0;
        return false;
    }

    // Push a main surface descriptor onto the surface stack.
    QGLPainterSurfaceInfo psurf;
    psurf.surface = surface;
    psurf.destroySurface = destroySurface;
    psurf.mainSurface = true;
    d_ptr->surfaceStack.append(psurf);

    // Force the matrices to be updated the first time we use them.
    d_ptr->modelViewMatrix.setDirty(true);
    d_ptr->projectionMatrix.setDirty(true);

    // Initialize the QOpenGLFunctions parent class.
    initializeGLFunctions(context);

    // Determine if the OpenGL implementation is fixed-function or not.
    d_ptr->isFixedFunction = !hasOpenGLFeature(QOpenGLFunctions::Shaders);
    return true;
}

/*!
    Begins painting on the GL context associated with \a widget.
    Returns false if \a widget is null.

    \sa end()
*/
bool QGLPainter::begin(QGLWidget *widget)
{
    if (!widget)
        return false;
    end();
    return begin(widget->context(), new QGLWidgetSurface(widget));
}

/*!
    Begins painting on the GL context associated with \a painter.
    Returns false if \a painter is not using an OpenGL paint engine.
    It is assumed that \a painter is the currently active painter
    and that it is associated with the current GL context.

    This function is typically used when mixing regular Qt painting
    operations and GL painting operations on a widget that is being
    drawn using the OpenGL graphics system.

    \sa end()
*/
bool QGLPainter::begin(QPainter *painter)
{
    // Validate that the painting is OpenGL-based.
    if (!painter)
        return false;
    QPaintEngine *engine = painter->paintEngine();
    if (!engine)
        return false;
    if (engine->type() != QPaintEngine::OpenGL &&
            engine->type() != QPaintEngine::OpenGL2)
        return false;

    // Begin GL painting operations.
    return begin(0, new QGLPainterSurface(painter));
}

/*!
    Begins painting to \a surface.  Returns false if \a surface is
    null or could not be activated.

    \sa end(), QGLAbstractSurface::activate()
*/
bool QGLPainter::begin(QGLAbstractSurface *surface)
{
    if (!surface)
        return false;
    end();
    return begin(0, surface, false);
}

/*!
    Ends GL painting.  Returns true if painting was ended successfully;
    false if this painter was not bound to a GL context.

    The GL context that was bound to this painter will not have
    QGLContext::doneCurrent() called on it.  It is the responsibility
    of the caller to terminate context operations.

    The effect() will be left active in the GL context and will be
    assumed to still be active the next time begin() is called.
    If this assumption doesn't apply, then call disableEffect()
    to disable the effect before calling end().

    This function will pop all surfaces from the surface stack,
    and return currentSurface() to null (the default drawing surface).

    \sa begin(), isActive(), disableEffect()
*/
bool QGLPainter::end()
{
    Q_D(QGLPainter);
    if (!d)
        return false;

    // Unbind the current vertex and index buffers.
    if (d->boundVertexBuffer) {
        QGLBuffer::release(QGLBuffer::VertexBuffer);
        d->boundVertexBuffer = 0;
    }
    if (d->boundIndexBuffer) {
        QGLBuffer::release(QGLBuffer::IndexBuffer);
        d->boundIndexBuffer = 0;
    }

    // Pop surfaces from the surface stack until we reach a
    // main surface.  Then deactivate the main surface.
    int size = d->surfaceStack.size();
    while (size > 0) {
        --size;
        QGLPainterSurfaceInfo &surf = d->surfaceStack[size];
        if (surf.mainSurface) {
            if (size > 0) {
                // There are still other surfaces on the stack, probably
                // because we are within a nested begin()/end() scope.
                // Re-activate the next surface down in the outer scope.
                QGLPainterSurfaceInfo &nextSurf = d->surfaceStack[size - 1];
                surf.surface->switchTo(nextSurf.surface);
            } else {
                // Last surface on the stack, so deactivate it permanently.
                surf.surface->deactivate();
            }
            if (surf.destroySurface)
                delete surf.surface;
            break;
        } else if (size > 0) {
            surf.surface->deactivate(d->surfaceStack[size - 1].surface);
        }
    }
    d->surfaceStack.resize(size);

    // Force a viewport update if we are within a nested begin()/end().
    d->updates |= UpdateViewport;

    // Destroy the QGLPainterPrivate if this is the last reference.
    if (!d->ref.deref())
        delete d;
    d_ptr = 0;
    return true;
}

/*!
    Returns true if this painter is currently bound to a GL context;
    false otherwise.

    \sa begin(), end()
*/
bool QGLPainter::isActive() const
{
    return (d_ptr != 0 && d_ptr->context != 0);
}

/*!
    Returns the GL context that is bound to this painter, or null
    if it is not currently bound.
*/
const QGLContext *QGLPainter::context() const
{
    if (d_ptr)
        return d_ptr->context;
    else
        return 0;
}

/*!
    Returns true if the underlying OpenGL implementation is OpenGL 1.x
    or OpenGL/ES 1.x and only supports fixed-function OpenGL operations.
    Returns false if the underlying OpenGL implementation is using
    GLSL or GLSL/ES shaders.

    If this function returns false, then the built-in effects will
    use shaders and QGLPainter will not update the fixed-function
    matrices in the OpenGL context when update() is called.
    User-supplied effects will need to use shaders also or update
    the fixed-function matrices themselves or call updateFixedFunction().

    \sa update(), updateFixedFunction()
*/
bool QGLPainter::isFixedFunction() const
{
#if defined(QT_OPENGL_ES_2)
    return false;
#else
    Q_D(const QGLPainter);
    if (d)
        return d->isFixedFunction;
    else
        return true;
#endif
}

/*!
    Sets the \a color to use to clear the color buffer when \c{glClear()}
    is called.
*/
void QGLPainter::setClearColor(const QColor& color)
{
    glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

/*!
    Sets the scissor \a rect for the current drawing surface
    to use when \c{GL_SCISSOR_TEST} is enabled.  If \a rect is empty,
    then the scissor will be set to clip away all drawing.

    Note that \a rect is in Qt co-ordinates with the origin
    at the top-left of the drawing surface's viewport rectangle.
    If the currentSurface() is an instance of QGLSubsurface,
    then \a rect will be adjusted relative to the subsurface's position.

    \sa currentSurface(), QGLAbstractSurface::viewportGL()
*/
void QGLPainter::setScissor(const QRect& rect)
{
    if (!rect.isEmpty()) {
        // Adjust the rectangle by the position of the surface viewport.
        QGLAbstractSurface *surface = currentSurface();
        QRect viewport = surface->viewportGL();
        QRect r(viewport.x() + rect.x(),
                viewport.y() + viewport.height() - (rect.y() + rect.height()),
                rect.width(), rect.height());
        if (!r.isEmpty())
            glScissor(r.x(), r.y(), r.width(), r.height());
        else
            glScissor(0, 0, 0, 0);
    } else {
        glScissor(0, 0, 0, 0);
    }
}

/*!
    Returns a reference to the projection matrix stack.

    It is recommended that setCamera() be used to set the projection
    matrix at the beginning of a scene rendering pass so that the
    eye position can be adjusted for stereo.

    \sa modelViewMatrix(), combinedMatrix(), setCamera()
*/
QMatrix4x4Stack& QGLPainter::projectionMatrix()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->projectionMatrix;
}

/*!
    Returns a reference to the modelview matrix stack.

    \sa projectionMatrix(), combinedMatrix(), normalMatrix(), setCamera()
    \sa worldMatrix()
*/
QMatrix4x4Stack& QGLPainter::modelViewMatrix()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->modelViewMatrix;
}

/*!
    \fn QMatrix4x4 QGLPainter::combinedMatrix() const

    Returns the result of multiplying the projectionMatrix()
    and the modelViewMatrix().  This combined matrix value is
    useful for setting uniform matrix values on shader programs.

    Calling this function is more efficient than calling
    projectionMatrix() and modelViewMatrix() separately and
    multiplying the return values.

    \sa projectionMatrix(), modelViewMatrix(), normalMatrix()
*/
QMatrix4x4 QGLPainter::combinedMatrix() const
{
    const QGLPainterPrivate *d = d_func();
    if (!d)
        return QMatrix4x4();
    const QMatrix4x4StackPrivate *proj = d->projectionMatrix.d_func();
    const QMatrix4x4StackPrivate *mv = d->modelViewMatrix.d_func();
    return proj->matrix * mv->matrix;
}

// Inverting the eye transformation will often result in values like
// 1.5e-15 in the world matrix.  Clamp these to zero to make worldMatrix()
// more stable when removing the eye component of the modelViewMatrix().
static inline qreal qt_gl_stablize_value(qreal value)
{
    return (qAbs(value) >= 0.00001f) ? value : 0.0f;
}
static inline QMatrix4x4 qt_gl_stablize_matrix(const QMatrix4x4 &m)
{
    return QMatrix4x4(qt_gl_stablize_value(m(0, 0)),
                      qt_gl_stablize_value(m(0, 1)),
                      qt_gl_stablize_value(m(0, 2)),
                      qt_gl_stablize_value(m(0, 3)),
                      qt_gl_stablize_value(m(1, 0)),
                      qt_gl_stablize_value(m(1, 1)),
                      qt_gl_stablize_value(m(1, 2)),
                      qt_gl_stablize_value(m(1, 3)),
                      qt_gl_stablize_value(m(2, 0)),
                      qt_gl_stablize_value(m(2, 1)),
                      qt_gl_stablize_value(m(2, 2)),
                      qt_gl_stablize_value(m(2, 3)),
                      qt_gl_stablize_value(m(3, 0)),
                      qt_gl_stablize_value(m(3, 1)),
                      qt_gl_stablize_value(m(3, 2)),
                      qt_gl_stablize_value(m(3, 3)));
}

/*!
    Returns the world matrix, which is the modelViewMatrix() without
    the eye transformation that was set in the previous call to
    setCamera().

    In the following example, the \c{world} variable will be set to the
    translation and scale component of the modelview transformation,
    without the "look at" component from the camera:

    \code
    painter.setCamera(camera);
    painter.modelViewMatrix().translate(0.0f, 5.0f, 0.0f);
    painter.modelViewMatrix().scale(1.5f);
    QMatrix4x4 world = painter.worldMatrix();
    \endcode

    Note: the world matrix is determined by multiplying the inverse of
    the camera's look at component with the current modelview matrix.
    Thus, the result may not be precisely the same as constructing a
    matrix from translate and scale operations starting with the identity.

    \sa modelViewMatrix(), setCamera()
*/
QMatrix4x4 QGLPainter::worldMatrix() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return qt_gl_stablize_matrix
        (d->inverseEyeMatrix * d->modelViewMatrix.top());
}

/*!
    \fn QMatrix3x3 QGLPainter::normalMatrix() const

    Returns the normal matrix corresponding to modelViewMatrix().

    The normal matrix is the transpose of the inverse of the top-left
    3x3 part of the 4x4 modelview matrix.  If the 3x3 sub-matrix is not
    invertible, this function returns the identity.

    \sa modelViewMatrix(), combinedMatrix()
*/
QMatrix3x3 QGLPainter::normalMatrix() const
{
    const QGLPainterPrivate *d = d_func();
    if (!d)
        return QMatrix3x3();
    const QMatrix4x4StackPrivate *mv = d->modelViewMatrix.d_func();
    return mv->matrix.normalMatrix();
}

/*!
    Returns the camera eye that is currently being used for stereo
    rendering.  The default is QGL::NoEye.

    The eye is used to adjust the camera position by a small amount
    when setCamera() is called.

    \sa setEye(), setCamera()
*/
QGL::Eye QGLPainter::eye() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->eye;
}

/*!
    Sets the camera \a eye that is currently being used for stereo
    rendering.

    The \a eye is used to adjust the camera position by a small amount
    when setCamera() is called.

    \sa eye(), setCamera()
*/
void QGLPainter::setEye(QGL::Eye eye)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->eye = eye;
}

/*!
    Sets the modelViewMatrix() and projectionMatrix() to the view
    defined by \a camera.  If eye() is not QGL::NoEye, then the view
    will be adjusted for the camera's eye separation.

    This function is typically called at the beginning of a scene rendering
    pass to initialize the modelview and projection matrices.

    \sa eye(), modelViewMatrix(), projectionMatrix(), worldMatrix()
*/
void QGLPainter::setCamera(const QGLCamera *camera)
{
    Q_ASSERT(camera);
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    QMatrix4x4 lookAt = camera->modelViewMatrix(d->eye);
    d->modelViewMatrix = lookAt;
    d->projectionMatrix = camera->projectionMatrix(aspectRatio());
    d->inverseEyeMatrix = lookAt.inverted();
}

/*!
    Returns true if \a point is outside the current viewing volume.
    This is used to perform object culling checks.
*/
bool QGLPainter::isCullable(const QVector3D& point) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    QVector3D projected = d->modelViewMatrix * point;
    projected = d->projectionMatrix * projected;
    return !d->viewingCube.contains(projected);
}

static inline uint outcode(const QVector4D &v)
{
    // For a discussion of outcodes see pg 388 Dunn & Parberry.
    // For why you can't just test if the point is in a bounding box
    // consider the case where a view frustum with view-size 1.5 x 1.5
    // is tested against a 2x2 box which encloses the near-plane, while
    // all the points in the box are outside the frustum.
    // TODO: optimise this with assembler - according to D&P this can
    // be done in one line of assembler on some platforms
    uint code = 0;
    if (v.x() < -v.w()) code |= 0x01;
    if (v.x() > v.w())  code |= 0x02;
    if (v.y() < -v.w()) code |= 0x04;
    if (v.y() > v.w())  code |= 0x08;
    if (v.z() < -v.w()) code |= 0x10;
    if (v.z() > v.w())  code |= 0x20;
    return code;
}

/*!
    Returns true if \a box is completely outside the current viewing volume.
    This is used to perform object culling checks.
*/
bool QGLPainter::isCullable(const QBox3D& box) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    // This function uses the technique of view frustum culling known as
    // clip space testing.  Since the normal QVector3D representation
    // of the points throws away the w value needed, we convert the box
    // into a set of 8 points represented as QVector4D's and then apply
    // the test.  The test is to transform the points into clip space
    // by applying the MV and Proj matrices, then test to see if the 4D
    // points are outside the clip space by testing x, y & z against w.
    QArray<QVector4D> box4d;
    QVector3D n = box.minimum();
    QVector3D x = box.maximum();
    box4d.append(QVector4D(n.x(), n.y(), x.z(), 1), QVector4D(x.x(), n.y(), x.z(), 1),
                 QVector4D(x.x(), x.y(), x.z(), 1), QVector4D(n.x(), x.y(), x.z(), 1));
    box4d.append(QVector4D(n.x(), n.y(), n.z(), 1), QVector4D(x.x(), n.y(), n.z(), 1),
                 QVector4D(x.x(), x.y(), n.z(), 1), QVector4D(n.x(), x.y(), n.z(), 1));
    QMatrix4x4 mvp = d->projectionMatrix.top() * d->modelViewMatrix.top();
    for (int i = 0; i < box4d.size(); ++i)
    {
        box4d[i] = mvp * box4d.at(i);
    }
    // if the logical AND of all the outcodes is non-zero then the BB is
    // definitely outside the view frustum.
    uint out = 0xff;
    for (int i = 0; i < box4d.size(); ++i)
    {
        out = out & outcode(box4d.at(i));
    }
    return out;
}

/*!
    Returns the current render order sequencer.

    \sa QGLRenderSequencer
*/
QGLRenderSequencer *QGLPainter::renderSequencer()
{
    Q_D(QGLPainter);
    if (!d->renderSequencer)
        d->renderSequencer = new QGLRenderSequencer(this);
    return d->renderSequencer;
}

/*!
    Returns the aspect ratio of the viewport for adjusting projection
    transformations.
*/
qreal QGLPainter::aspectRatio() const
{
    return currentSurface()->aspectRatio();
}

/*!
    Returns the current effect that is in use, which is userEffect()
    if it is not null, or the effect object associated with
    standardEffect() otherwise.

    If isPicking() is true, then this will return the effect object
    that is being used to generate pick colors.

    \sa userEffect(), standardEffect(), isPicking()
*/
QGLAbstractEffect *QGLPainter::effect() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(const_cast<QGLPainter *>(this));
    return d->effect;
}

/*!
    Returns the user-defined effect that is being used for drawing
    operations, or null if standardEffect() is in use.

    \sa setUserEffect(), standardEffect(), effect()
*/
QGLAbstractEffect *QGLPainter::userEffect() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->userEffect;
}

/*!
    Sets a user-defined \a effect to use for drawing operations
    in the current GL context.  If \a effect is null, this will
    disable user-defined effects and return to using standardEffect().

    \sa effect(), draw(), setStandardEffect()
*/
void QGLPainter::setUserEffect(QGLAbstractEffect *effect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->userEffect == effect)
        return;
    if (d->effect)
        d->effect->setActive(this, false);
    d->userEffect = effect;
    if (effect && (!d->pick || !d->pick->isPicking)) {
        d->effect = effect;
        d->effect->setActive(this, true);
        d->updates = UpdateAll;
    } else {
        // Revert to the effect associated with standardEffect().
        d->effect = 0;
        d->ensureEffect(this);
    }
}

/*!
    Returns the standard effect to use for rendering fragments in
    the current GL context when userEffect() is null.

    \sa setStandardEffect(), userEffect()
*/
QGL::StandardEffect QGLPainter::standardEffect() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->standardEffect;
}

/*!
    Sets a standard \a effect to use for rendering fragments
    in the current GL context.  This will also set userEffect()
    to null.  If \a effect is an invalid value, then the behavior
    of QGL::FlatColor will be used instead.

    \sa standardEffect(), setUserEffect()
*/
void QGLPainter::setStandardEffect(QGL::StandardEffect effect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->standardEffect == effect && d->effect && d->userEffect == 0)
        return;
    if (d->effect)
        d->effect->setActive(this, false);
    d->standardEffect = effect;
    d->userEffect = 0;
    d->effect = 0;
    d->ensureEffect(this);
}

/*!
    Disables the current effect and sets userEffect() to null.
    Unlike setUserEffect() this not activate the standardEffect()
    until the next time effect() is called.

    This function can be used to disable all effect-based drawing
    operations prior to performing raw GL calls.  The next time
    effect() is called on this QGLPainter, the standardEffect()
    will be reactivated.  An effect can also be reactivated by
    calling setUserEffect() or setStandardEffect().

    \sa userEffect(), standardEffect()
*/
void QGLPainter::disableEffect()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->effect)
        d->effect->setActive(this, false);
    d->userEffect = 0;
    d->effect = 0;
}

/*!
    Returns the cached shader program associated with \a name; or null
    if \a name is not currently associated with a shader program.

    \sa setCachedProgram()
*/
QGLShaderProgram *QGLPainter::cachedProgram(const QString& name) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->cachedPrograms.value(name, 0);
}

/*!
    Sets the cached shader \a program associated with \a name.

    Effect objects can use this function to store pre-compiled
    and pre-linked shader programs in the painter for future
    use by the same effect.  The \a program will be destroyed
    when context() is destroyed.

    If \a program is null, then the program associated with \a name
    will be destroyed.  If \a name is already present as a cached
    program, then it will be replaced with \a program.

    Names that start with "\c{qt.}" are reserved for use by Qt's
    internal effects.

    \sa cachedProgram()
*/
void QGLPainter::setCachedProgram
    (const QString& name, QGLShaderProgram *program)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    QGLShaderProgram *current = d->cachedPrograms.value(name, 0);
    if (current != program) {
        if (program)
            d->cachedPrograms[name] = program;
        else
            d->cachedPrograms.remove(name);
        delete current;
    }
}

void QGLPainterPrivate::createEffect(QGLPainter *painter)
{
    if (userEffect) {
        if (!pick || !pick->isPicking) {
            effect = userEffect;
            effect->setActive(painter, true);
            updates = QGLPainter::UpdateAll;
            return;
        }
        if (userEffect->supportsPicking()) {
            effect = userEffect;
            effect->setActive(painter, true);
            updates = QGLPainter::UpdateAll;
            return;
        }
        effect = pick->defaultPickEffect;
        effect->setActive(painter, true);
        updates = QGLPainter::UpdateAll;
        return;
    }
    if (uint(standardEffect) >= QGL_MAX_STD_EFFECTS)
        effect = stdeffects[int(QGL::FlatColor)];
    else
        effect = stdeffects[int(standardEffect)];
    if (!effect) {
        switch (standardEffect) {
        case QGL::FlatColor: default:
            effect = new QGLFlatColorEffect();
            break;
        case QGL::FlatPerVertexColor:
            effect = new QGLPerVertexColorEffect();
            break;
        case QGL::FlatReplaceTexture2D:
            effect = new QGLFlatTextureEffect();
            break;
        case QGL::FlatDecalTexture2D:
            effect = new QGLFlatDecalTextureEffect();
            break;
        case QGL::LitMaterial:
            effect = new QGLLitMaterialEffect();
            break;
        case QGL::LitDecalTexture2D:
            effect = new QGLLitDecalTextureEffect();
            break;
        case QGL::LitModulateTexture2D:
            effect = new QGLLitModulateTextureEffect();
            break;
        }
        if (uint(standardEffect) >= QGL_MAX_STD_EFFECTS)
            stdeffects[int(QGL::FlatColor)] = effect;
        else
            stdeffects[int(standardEffect)] = effect;
    }
    if (!pick || !pick->isPicking || effect->supportsPicking()) {
        effect->setActive(painter, true);
    } else {
        effect = pick->defaultPickEffect;
        effect->setActive(painter, true);
    }
    updates = QGLPainter::UpdateAll;
}

/*!
    Returns the last color that was set with setColor().  The default
    value is (1, 1, 1, 1).

    \sa setColor()
*/
QColor QGLPainter::color() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->color;
}

/*!
    Sets the default fragment \a color for effects associated
    with this painter.  This function does not apply the color
    to the effect until update() is called.

    \sa color(), update()
*/
void QGLPainter::setColor(const QColor& color)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->color = color;
    d->updates |= UpdateColor;
}

static void qt_gl_setVertexAttribute(QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
#if !defined(QT_OPENGL_ES_2)
    switch (attribute) {
    case QGL::Position:
        glVertexPointer(value.tupleSize(), value.type(),
                        value.stride(), value.data());
        break;

    case QGL::Normal:
        if (value.tupleSize() == 3)
            glNormalPointer(value.type(), value.stride(), value.data());
        break;

    case QGL::Color:
        glColorPointer(value.tupleSize(), value.type(),
                       value.stride(), value.data());
        break;

#ifdef GL_TEXTURE_COORD_ARRAY
    case QGL::TextureCoord0:
    case QGL::TextureCoord1:
    case QGL::TextureCoord2:
    {
        int unit = (int)(attribute - QGL::TextureCoord0);
        qt_gl_ClientActiveTexture(GL_TEXTURE0 + unit);
        glTexCoordPointer(value.tupleSize(), value.type(),
                          value.stride(), value.data());
        if (unit != 0)  // Stay on unit 0 between requests.
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
    }
    break;
#endif

    default: break;
    }
#else
    Q_UNUSED(attribute);
    Q_UNUSED(value);
#endif
}

/*!
    Returns the set of vertex attributes that have been set on the
    painter state by setVertexAttribute() and setVertexBundle()
    since the last call to clearAttributes().

    The most common use for this function is to determine if specific
    attributes have been supplied on the painter so as to adjust the
    current drawing effect accordingly.  The following example will
    use a lit texture effect if texture co-ordinates were provided
    in the vertex bundle, or a simple lit material effect if
    texture co-ordinates were not provided:

    \code
    painter.clearAttributes();
    painter.setVertexBundle(bundle);
    if (painter.attributes().contains(QGL::TextureCoord0))
        painter.setStandardEffect(QGL::LitModulateTexture2D);
    else
        painter.setStandardEffect(QGL::LitMaterial);
    \endcode

    It is important to clear the attributes before setting the vertex
    bundle, so that attributes from a previous bundle will not leak
    through.  Multiple vertex bundles may be supplied if they contain
    different parts of the same logical piece of geometry.

    \sa clearAttributes(), setVertexBundle()
*/
QGLAttributeSet QGLPainter::attributes() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->attributeSet;
}

/*!
    Clears the set of vertex attributes that have been set on the
    painter state by setVertexAttribute() and setVertexBundle().
    See the documentation for attributes() for more information.

    \sa attributes()
*/
void QGLPainter::clearAttributes()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->attributeSet.clear();
}

/*!
    Sets a vertex \a attribute on the current GL context to \a value.

    The vertex attribute is bound to the GL state on the index
    corresponding to \a attribute.  For example, QGL::Position
    will be bound to index 0, QGL::TextureCoord0 will be bound
    to index 3, etc.

    Vertex attributes are independent of the effect() and can be
    bound once and then used with multiple effects.

    If this is the first attribute in a new piece of geometry,
    it is recommended that clearAttributes() be called before this
    function.  This will inform QGLPainter that a new piece of geometry
    is being provided and that the previous geometry is now invalid.
    See the documentation for attributes() for more information.

    \sa setVertexBundle(), draw(), clearAttributes(), attributes()
*/
void QGLPainter::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(this);
    if (d->boundVertexBuffer) {
        QGLBuffer::release(QGLBuffer::VertexBuffer);
        d->boundVertexBuffer = 0;
    }
    if (d->isFixedFunction) {
        qt_gl_setVertexAttribute(attribute, value);
    } else {
        glVertexAttribPointer(GLuint(attribute), value.tupleSize(),
                              value.type(), GL_TRUE,
                              value.stride(), value.data());
    }
    d->attributeSet.insert(attribute);
}

/*!
    Sets the vertex attributes on the current GL context that are
    stored in \a buffer.

    The vertex attributes are bound to the GL state on the indexes
    that are specified within \a buffer; QGL::Position will be
    bound to index 0, QGL::TextureCoord0 will be bound to index 3, etc.

    Vertex attributes are independent of the effect() and can be
    bound once and then used with multiple effects.

    It is recommended that clearAttributes() be called before this
    function to inform QGLPainter that a new piece of geometry is
    being provided and that the previous geometry is now invalid.
    See the documentation for attributes() for more information.

    \sa setVertexAttribute(), draw(), clearAttributes(), attributes()
*/
void QGLPainter::setVertexBundle(const QGLVertexBundle& buffer)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(this);
    QGLVertexBundlePrivate *bd = const_cast<QGLVertexBundlePrivate *>(buffer.d_func());
    if (bd->buffer.isCreated()) {
        GLuint id = bd->buffer.bufferId();
        if (id != d->boundVertexBuffer) {
            bd->buffer.bind();
            d->boundVertexBuffer = id;
        }
    } else if (d->boundVertexBuffer) {
        QGLBuffer::release(QGLBuffer::VertexBuffer);
        d->boundVertexBuffer = 0;
    }
    for (int index = 0; index < bd->attributes.size(); ++index) {
        QGLVertexBundleAttribute *attr = bd->attributes[index];
        if (d->isFixedFunction) {
            qt_gl_setVertexAttribute(attr->attribute, attr->value);
        } else {
            glVertexAttribPointer(GLuint(attr->attribute),
                                  attr->value.tupleSize(),
                                  attr->value.type(), GL_TRUE,
                                  attr->value.stride(), attr->value.data());
        }
    }
    d->attributeSet.unite(buffer.attributes());
}

/*!
    Updates the projection matrix, modelview matrix, and lighting
    conditions in the currently active effect() object by calling
    QGLAbstractEffect::update().  Also updates \c{glViewport()}
    to cover the currentSurface() if necessary.

    Normally this function is called automatically by draw().
    However, if the user wishes to use raw GL functions to draw fragments,
    it will be necessary to explicitly call this function to ensure that
    the matrix state and lighting conditions have been set on the
    active effect().

    Note that this function informs the effect that an update is needed.
    It does not change the GL state itself, except for \c{glViewport()}.
    In particular, the modelview and projection matrices in the
    fixed-function pipeline are not changed unless the effect or
    application calls updateFixedFunction().

    \sa setUserEffect(), projectionMatrix(), modelViewMatrix()
    \sa draw(), updateFixedFunction()
*/
void QGLPainter::update()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(this);
    QGLPainter::Updates updates = d->updates;
    d->updates = 0;
    if (d->modelViewMatrix.isDirty()) {
        updates |= UpdateModelViewMatrix;
        d->modelViewMatrix.setDirty(false);
    }
    if (d->projectionMatrix.isDirty()) {
        updates |= UpdateProjectionMatrix;
        d->projectionMatrix.setDirty(false);
    }
    if ((updates & UpdateViewport) != 0) {
        QRect viewport = currentSurface()->viewportGL();
        glViewport(viewport.x(), viewport.y(),
                   viewport.width(), viewport.height());
    }
    if (updates != 0)
        d->effect->update(this, updates);
}

#if !defined(QT_OPENGL_ES_2)

static void setLight(int light, const QGLLightParameters *parameters,
                     const QMatrix4x4& transform)
{
    GLfloat params[4];

    QColor color = parameters->ambientColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_AMBIENT, params);

    color = parameters->diffuseColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_DIFFUSE, params);

    color = parameters->specularColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_SPECULAR, params);

    QVector4D vector = parameters->eyePosition(transform);
    params[0] = vector.x();
    params[1] = vector.y();
    params[2] = vector.z();
    params[3] = vector.w();
    glLightfv(light, GL_POSITION, params);

    QVector3D spotDirection = parameters->eyeSpotDirection(transform);
    params[0] = spotDirection.x();
    params[1] = spotDirection.y();
    params[2] = spotDirection.z();
    glLightfv(light, GL_SPOT_DIRECTION, params);

    params[0] = parameters->spotExponent();
    glLightfv(light, GL_SPOT_EXPONENT, params);

    params[0] = parameters->spotAngle();
    glLightfv(light, GL_SPOT_CUTOFF, params);

    params[0] = parameters->constantAttenuation();
    glLightfv(light, GL_CONSTANT_ATTENUATION, params);

    params[0] = parameters->linearAttenuation();
    glLightfv(light, GL_LINEAR_ATTENUATION, params);

    params[0] = parameters->quadraticAttenuation();
    glLightfv(light, GL_QUADRATIC_ATTENUATION, params);
}

static void setMaterial(int face, const QGLMaterial *parameters)
{
    GLfloat params[17];

    QColor mcolor = parameters->ambientColor();
    params[0] = mcolor.redF();
    params[1] = mcolor.greenF();
    params[2] = mcolor.blueF();
    params[3] = mcolor.alphaF();

    mcolor = parameters->diffuseColor();
    params[4] = mcolor.redF();
    params[5] = mcolor.greenF();
    params[6] = mcolor.blueF();
    params[7] = mcolor.alphaF();

    mcolor = parameters->specularColor();
    params[8] = mcolor.redF();
    params[9] = mcolor.greenF();
    params[10] = mcolor.blueF();
    params[11] = mcolor.alphaF();

    mcolor = parameters->emittedLight();
    params[12] = mcolor.redF();
    params[13] = mcolor.greenF();
    params[14] = mcolor.blueF();
    params[15] = mcolor.alphaF();

    params[16] = parameters->shininess();

    glMaterialfv(face, GL_AMBIENT, params);
    glMaterialfv(face, GL_DIFFUSE, params + 4);
    glMaterialfv(face, GL_SPECULAR, params + 8);
    glMaterialfv(face, GL_EMISSION, params + 12);
    glMaterialfv(face, GL_SHININESS, params + 16);
}

#endif // !QT_OPENGL_ES_2

/*!
    Updates the fixed-function pipeline with the current painting
    state according to the flags in \a updates.

    This function is intended for use by effects in their
    QGLAbstractEffect::update() override if they are using the
    fixed-function pipeline.  It can also be used by user
    applications if they need the QGLPainter state to be
    set in the fixed-function pipeline.

    If the OpenGL implementation does not have a fixed-function
    pipeline, e.g. OpenGL/ES 2.0, this function does nothing.

    \sa update()
*/
void QGLPainter::updateFixedFunction(QGLPainter::Updates updates)
{
#if defined(QT_OPENGL_ES_2)
    Q_UNUSED(updates);
#else
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color;
        if (isPicking())
            color = pickColor();
        else
            color = this->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
    if ((updates & QGLPainter::UpdateModelViewMatrix) != 0) {
        const QMatrix4x4 &matrix = d->modelViewMatrix.top();
        glMatrixMode(GL_MODELVIEW);
        if (sizeof(qreal) == sizeof(GLfloat)) {
            glLoadMatrixf(reinterpret_cast<const GLfloat *>
                (matrix.constData()));
        } else {
            GLfloat mat[16];
            const qreal *m = matrix.constData();
            for (int index = 0; index < 16; ++index)
                mat[index] = m[index];
            glLoadMatrixf(mat);
        }
    }
    if ((updates & QGLPainter::UpdateProjectionMatrix) != 0) {
        const QMatrix4x4 &matrix = d->projectionMatrix.top();
        glMatrixMode(GL_PROJECTION);
        if (sizeof(qreal) == sizeof(GLfloat)) {
            glLoadMatrixf(reinterpret_cast<const GLfloat *>
                (matrix.constData()));
        } else {
            GLfloat mat[16];
            const qreal *m = matrix.constData();
            for (int index = 0; index < 16; ++index)
                mat[index] = m[index];
            glLoadMatrixf(mat);
        }
    }
    if ((updates & QGLPainter::UpdateLights) != 0) {
        // Save the current modelview matrix and load the identity.
        // We need to apply the light in the modelview transformation
        // that was active when the light was specified.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Enable the main light.
        const QGLLightParameters *params = mainLight();
        setLight(GL_LIGHT0, params, mainLightTransform());

        // Restore the previous modelview transformation.
        glPopMatrix();

        // Set up the light model parameters if at least one light is enabled.
        const QGLLightModel *lightModel = this->lightModel();
        GLfloat values[4];
#ifdef GL_LIGHT_MODEL_TWO_SIDE
        if (lightModel->model() == QGLLightModel::TwoSided)
            values[0] = 1.0f;
        else
            values[0] = 0.0f;
        glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, values);
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL
        if (lightModel->colorControl() == QGLLightModel::SeparateSpecularColor)
            values[0] = GL_SEPARATE_SPECULAR_COLOR;
        else
            values[0] = GL_SINGLE_COLOR;
        glLightModelfv(GL_LIGHT_MODEL_COLOR_CONTROL, values);
#endif
#ifdef GL_LIGHT_MODEL_LOCAL_VIEWER
        if (lightModel->viewerPosition() == QGLLightModel::LocalViewer)
            values[0] = 1.0f;
        else
            values[0] = 0.0f;
        glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, values);
#endif
#ifdef GL_LIGHT_MODEL_AMBIENT
        QColor color = lightModel->ambientSceneColor();
        values[0] = color.redF();
        values[1] = color.blueF();
        values[2] = color.greenF();
        values[3] = color.alphaF();
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, values);
#endif
    }
    if ((updates & QGLPainter::UpdateMaterials) != 0) {
        const QGLMaterial *frontMaterial = faceMaterial(QGL::FrontFaces);
        const QGLMaterial *backMaterial = faceMaterial(QGL::BackFaces);
        if (frontMaterial == backMaterial) {
            setMaterial(GL_FRONT_AND_BACK, frontMaterial);
        } else {
            setMaterial(GL_FRONT, frontMaterial);
            setMaterial(GL_BACK, backMaterial);
        }
    }
#endif
}

/*!
    Draws primitives using \a count vertices from the arrays specified
    by setVertexAttribute().  The type of primitive to draw is specified
    by \a mode.

    This operation will consume \a count values from the
    enabled arrays, starting at \a index.

    \sa update()
*/
void QGLPainter::draw(QGL::DrawingMode mode, int count, int index)
{
    update();
    glDrawArrays((GLenum)mode, index, count);
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexAttribute().  The type of primitive to draw is
    specified by \a mode.

    This operation will consume \a count elements of \a indices,
    which are used to index into the enabled arrays.

    \sa update()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const ushort *indices, int count)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    update();
    if (d->boundIndexBuffer) {
        QGLBuffer::release(QGLBuffer::IndexBuffer);
        d->boundIndexBuffer = 0;
    }
    glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT, indices);
}

/*!
    Pushes \a surface onto the surface stack and makes it the current
    drawing surface for context().  If \a surface is null, then the
    current drawing surface will be set to the main surface (e.g. the window).

    Note: the \a surface object must remain valid until popped from
    the stack or end() is called.  All surfaces are popped from
    the stack by end().

    The UpdateViewport flag will be set to indicate that the
    \c{glViewport()} should be adjusted to the extents of \a surface
    when update() is next called.

    \sa popSurface(), currentSurface(), setSurface()
    \sa QGLAbstractSurface::activate()
*/
void QGLPainter::pushSurface(QGLAbstractSurface *surface)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    Q_ASSERT(surface);
    if (!surface) {
        // Find the most recent main surface for this painter.
        int size = d->surfaceStack.size();
        while (size > 0 && !d->surfaceStack[size - 1].mainSurface)
            --size;
        if (!size)
            return;     // Shouldn't happen, but be safe anyway.
        surface = d->surfaceStack[size - 1].surface;
    }
    Q_ASSERT(!d->surfaceStack.isEmpty()); // Should have a main surface.
    QGLAbstractSurface *current = d->surfaceStack.top().surface;
    QGLPainterSurfaceInfo psurf;
    psurf.surface = surface;
    psurf.destroySurface = false;
    psurf.mainSurface = false;
    d->surfaceStack.append(psurf);
    current->switchTo(surface);
    d->updates |= UpdateViewport;
}

/*!
    Pops the top-most drawing surface from the surface stack
    and returns it.  The next object on the stack will be made
    the current drawing surface for context().  Returns null if the
    surface stack is already at the main surface (e.g. the window).

    The UpdateViewport flag will be set to indicate that the
    \c{glViewport()} should be adjusted to the new surface extents
    when update() is next called.

    \sa pushSurface(), currentSurface(), setSurface()
*/
QGLAbstractSurface *QGLPainter::popSurface()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    Q_ASSERT(!d->surfaceStack.isEmpty()); // Should have a main surface.
    QGLPainterSurfaceInfo &surf = d->surfaceStack.top();
    if (surf.mainSurface)
        return 0;
    QGLAbstractSurface *surface = surf.surface;
    d->surfaceStack.pop();
    Q_ASSERT(!d->surfaceStack.isEmpty()); // Should have a main surface.
    QGLAbstractSurface *nextSurface = d->surfaceStack.top().surface;
    surface->switchTo(nextSurface);
    d->updates |= UpdateViewport;
    return surface;
}

/*!
    Sets the top-most drawing surface on the surface stack to \a surface
    and activate it.

    Note: if the top-most drawing surface is the main surface specified
    during begin(), then this function will perform a pushSurface()
    instead.  Typically this function is used to replace the last
    surface that was pushed onto the stack and avoid doing popSurface()
    followed by pushSurface().  The main surface cannot be replaced
    in this manner.

    The UpdateViewport flag will be set to indicate that the
    \c{glViewport()} should be adjusted to the extents of \a surface
    when update() is next called.

    \sa pushSurface(), popSurface(), currentSurface()
*/
void QGLPainter::setSurface(QGLAbstractSurface *surface)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    Q_ASSERT(surface);
    Q_ASSERT(!d->surfaceStack.isEmpty()); // Should have a main surface.
    QGLPainterSurfaceInfo &surf = d->surfaceStack.top();
    if (surf.mainSurface) {
        pushSurface(surface);
        return;
    }
    QGLAbstractSurface *oldSurface = surf.surface;
    surf.surface = surface;
    oldSurface->switchTo(surface);
    d->updates |= UpdateViewport;
}

/*!
    Returns the current drawing surface.

    \sa pushSurface(), popSurface(), setSurface()
*/
QGLAbstractSurface *QGLPainter::currentSurface() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    Q_ASSERT(!d->surfaceStack.isEmpty()); // Should have a main surface.
    return d->surfaceStack.top().surface;
}

/*!
    Returns the current lighting model.

    \sa setLightModel()
*/
const QGLLightModel *QGLPainter::lightModel() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->lightModel) {
        if (!d->defaultLightModel)
            d->defaultLightModel = new QGLLightModel();
        d->lightModel = d->defaultLightModel;
    }
    return d->lightModel;
}

/*!
    Sets the current lighting model to \a value.  If \a value is
    null, then the default lighting model parameters will be used.

    The light settings in the GL server will not be changed until
    update() is called.

    \sa lightModel()
*/
void QGLPainter::setLightModel(const QGLLightModel *value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->lightModel = value;
    d->updates |= QGLPainter::UpdateLights;
}

/*!
    Returns the parameters for the main light in the scene.

    The light parameters are specified in world co-ordinates at
    the point when setMainLight() was called.  The mainLightTransform()
    must be applied to obtain eye co-ordinates.

    This function is a convenience that returns the light with
    identifier 0.  If light 0 is not currently enabled, then a
    default light is added to the painter with an identity
    transform and then returned as the main light.

    \sa setMainLight(), mainLightTransform(), addLight()
*/
const QGLLightParameters *QGLPainter::mainLight() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->lights.isEmpty()) {
        if (!d->defaultLight)
            d->defaultLight = new QGLLightParameters();
        d->lights.append(d->defaultLight);
        d->lightTransforms.append(QMatrix4x4());
    } else if (!d->lights[0]) {
        if (!d->defaultLight)
            d->defaultLight = new QGLLightParameters();
        d->lights[0] = d->defaultLight;
        d->lightTransforms[0] = QMatrix4x4();
    }
    return d->lights[0];
}

/*!
    Sets the \a parameters for the main light in the scene.
    The mainLightTransform() is set to the current modelViewMatrix().

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The mainLightTransform() specifies the transformation to apply to
    convert the world co-ordinates into eye co-ordinates when the light
    is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    This function is a convenience that sets the light with
    identifier 0.  If \a parameters is null, then light 0
    will be removed.

    \sa mainLight(), mainLightTransform(), addLight()
*/
void QGLPainter::setMainLight(const QGLLightParameters *parameters)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->lights.isEmpty()) {
        if (parameters) {
            d->lights.append(parameters);
            d->lightTransforms.append(modelViewMatrix());
            d->updates |= QGLPainter::UpdateLights;
        }
    } else if (parameters) {
        d->lights[0] = parameters;
        d->lightTransforms[0] = modelViewMatrix();
        d->updates |= QGLPainter::UpdateLights;
    } else {
        removeLight(0);
    }
}

/*!
    Sets the \a parameters for the main light in the scene, and set
    mainLightTransform() to \a transform.

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The \a transform specifies the transformation to apply to convert the
    world co-ordinates into eye co-ordinates when the light is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    This function is a convenience that sets the light with
    identifier 0.  If \a parameters is null, then light 0
    will be removed.

    \sa mainLight(), mainLightTransform()
*/
void QGLPainter::setMainLight
        (const QGLLightParameters *parameters, const QMatrix4x4& transform)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->lights.isEmpty()) {
        if (parameters) {
            d->lights.append(parameters);
            d->lightTransforms.append(transform);
            d->updates |= QGLPainter::UpdateLights;
        }
    } else if (parameters) {
        d->lights[0] = parameters;
        d->lightTransforms[0] = transform;
        d->updates |= QGLPainter::UpdateLights;
    } else {
        removeLight(0);
    }
}

/*!
    Returns the modelview transformation matrix for the main light that
    was set at the time setMainLight() was called.

    The light transform may be used by later painting operations to
    convert the light from world co-ordinates into eye co-ordinates.
    The eye transformation is set when the light is specified.

    This function is a convenience that returns the tranform for the
    light with identifier 0.  If light 0 is not enabled, then the
    function returns the identity matrix.

    \sa mainLight(), setMainLight(), addLight()
*/
QMatrix4x4 QGLPainter::mainLightTransform() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->lights.isEmpty() && d->lights[0])
        return d->lightTransforms[0];
    else
        return QMatrix4x4();
}

/*!
    Adds a light to this painter, with the specified \a parameters.
    The lightTransform() for the light is set to the current
    modelViewMatrix().  Returns an identifier for the light.

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The lightTransform() specifies the transformation to apply to
    convert the world co-ordinates into eye co-ordinates when the light
    is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    \sa removeLight(), light(), mainLight()
*/
int QGLPainter::addLight(const QGLLightParameters *parameters)
{
    return addLight(parameters, modelViewMatrix());
}

/*!
    Adds a light to this painter, with the specified \a parameters.
    The lightTransform() for the light is set to \a transform.
    Returns an identifier for the light.

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The \a transform specifies the transformation to apply to
    convert the world co-ordinates into eye co-ordinates when the light
    is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    \sa removeLight(), light(), mainLight()
*/
int QGLPainter::addLight(const QGLLightParameters *parameters, const QMatrix4x4 &transform)
{
    Q_ASSERT(parameters);
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    int lightId = 0;
    while (lightId < d->lights.size() && d->lights[lightId] != 0)
        ++lightId;
    if (lightId < d->lights.size()) {
        d->lights[lightId] = parameters;
        d->lightTransforms[lightId] = transform;
    } else {
        d->lights.append(parameters);
        d->lightTransforms.append(transform);
    }
    d->updates |= QGLPainter::UpdateLights;
    return lightId;
}

/*!
    Removes the light with the specified \a lightId.

    \sa addLight(), light()
*/
void QGLPainter::removeLight(int lightId)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (lightId >= 0 && lightId < d->lights.size()) {
        d->lights[lightId] = 0;
        if (lightId >= (d->lights.size() - 1)) {
            do {
                d->lights.resize(lightId);
                d->lightTransforms.resize(lightId);
                --lightId;
            } while (lightId >= 0 && d->lights[lightId] == 0);
        }
        d->updates |= QGLPainter::UpdateLights;
    }
}

/*!
    Returns the maximum light identifier currently in use on this painter;
    or -1 if there are no lights.

    It is possible that some light identifiers less than maximumLightId()
    may be invalid because the lights have been removed.  Use the following
    code to locate all enabled lights:

    \code
    int maxLightId = painter.maximumLightId();
    for (int lightId = 0; index <= maxLightId; ++index) {
        const QGLLightParameters *params = painter.light(lightId);
        if (params) {
            ...
        }
    }
    \endcode

    \sa addLight(), light()
*/
int QGLPainter::maximumLightId() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->lights.size() - 1;
}

/*!
    Returns the parameters for the light with the identifier \a lightId;
    or null if \a lightId is not valid or has been removed.

    \sa addLight(), removeLight(), lightTransform()
*/
const QGLLightParameters *QGLPainter::light(int lightId) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (lightId >= 0 && lightId < d->lights.size())
        return d->lights[lightId];
    else
        return 0;
}

/*!
    Returns the modelview transformation for the light with the identifier
    \a lightId; or the identity matrix if \a lightId is not valid or has
    been removed.

    \sa addLight(), removeLight(), light()
*/
QMatrix4x4 QGLPainter::lightTransform(int lightId) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (lightId >= 0 && lightId < d->lights.size() && d->lights[lightId])
        return d->lightTransforms[lightId];
    else
        return QMatrix4x4();
}

/*!
    Returns the material that is used for drawing \a face on polygons.
    If \a face is QGL::FrontFaces or QGL::AllFaces, then the front
    material is returned.  If \a face is QGL::BackFaces, then the
    back material is returned.

    \sa setFaceMaterial(), setFaceColor()
*/
const QGLMaterial *QGLPainter::faceMaterial(QGL::Face face) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (face == QGL::BackFaces) {
        if (!d->backMaterial) {
            if (!d->defaultMaterial)
                d->defaultMaterial = new QGLMaterial();
            d->backMaterial = d->defaultMaterial;
        }
        return d->backMaterial;
    } else {
        if (!d->frontMaterial) {
            if (!d->defaultMaterial)
                d->defaultMaterial = new QGLMaterial();
            d->frontMaterial = d->defaultMaterial;
        }
        return d->frontMaterial;
    }
}

/*!
    Sets the material that is used for drawing \a face on polygons
    to \a value.  If \a face is QGL::FrontFaces, then the front
    material is set.  If \a face is QGL::BackFaces, then the
    back material is set.  If \a face is QGL::AllFaces, then both
    the front and back materials are set.

    If \a value is null, then the \a face material will be set to
    the default material properties.

    The material settings in the GL server will not be changed until
    update() is called.

    \sa faceMaterial(), setFaceColor()
*/
void QGLPainter::setFaceMaterial
        (QGL::Face face, const QGLMaterial *value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (face == QGL::FrontFaces) {
        if (d->frontMaterial == value)
            return;
        d->frontMaterial = value;
    } else if (face == QGL::BackFaces) {
        if (d->backMaterial == value)
            return;
        d->backMaterial = value;
    } else {
        if (d->frontMaterial == value && d->backMaterial == value)
            return;
        d->frontMaterial = value;
        d->backMaterial = value;
    }
    d->updates |= QGLPainter::UpdateMaterials;
}

static QGLMaterial *createColorMaterial
    (QGLMaterial *prev, const QColor& color)
{
    QGLMaterial *material;
    if (prev)
        material = prev;
    else
        material = new QGLMaterial();
    material->setColor(color);
    return material;
}

/*!
    Sets the material that is used for drawing \a face on polygons
    to \a color.  This is a convenience function for setting materials
    to simple colors.

    The RGB components of the ambient color of the material will be set
    to 20% of \a color, and the RGB components of the diffuse color of the
    material will be set to 80% of \a color.  The alpha components of
    the ambient and diffuse material colors will both be set to the
    alpha component of \a color.

    If \a face is QGL::FrontFaces, then the front material is set.
    If \a face is QGL::BackFaces, then the back material is set.
    If \a face is QGL::AllFaces, then both the front and back
    materials are set.

    The material settings in the GL server will not be changed until
    update() is called.

    \sa faceMaterial(), setFaceMaterial()
*/
void QGLPainter::setFaceColor(QGL::Face face, const QColor& color)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (face == QGL::FrontFaces) {
        d->frontColorMaterial =
            createColorMaterial(d->frontColorMaterial, color);
        d->frontMaterial = d->frontColorMaterial;
    } else if (face == QGL::BackFaces) {
        d->backColorMaterial =
            createColorMaterial(d->backColorMaterial, color);
        d->backMaterial = d->backColorMaterial;
    } else {
        d->frontColorMaterial =
            createColorMaterial(d->frontColorMaterial, color);
        d->backColorMaterial =
            createColorMaterial(d->backColorMaterial, color);
        d->frontMaterial = d->frontColorMaterial;
        d->backMaterial = d->backColorMaterial;
    }
    d->updates |= QGLPainter::UpdateMaterials;
}

/*!
    Returns true if this painter is in object picking mode;
    false if this painter is in normal rendering mode.

    \sa setPicking(), objectPickId()
*/
bool QGLPainter::isPicking() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return (d->pick ? d->pick->isPicking : false);
}

/*!
    Enables or disables object picking mode according to \a value.

    If \a value is true, then the effect() will be overridden with a
    simple flat color effect that renders objects with pickColor().
    These colors can be read back later with pickObject().

    \sa isPicking(), objectPickId(), pickObject()
*/
void QGLPainter::setPicking(bool value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->pick)
        d->pick = new QGLPainterPickPrivate();
    if (d->pick->isPicking != value) {
        // Switch to/from the pick effect.
        d->pick->isPicking = value;
        if (d->effect)
            d->effect->setActive(this, false);
        d->effect = 0;
        d->ensureEffect(this);
    }
}

/*!
    Returns the current object pick identifier.  The default value
    is -1 which indicates that rendered objects should not have a
    pickColor() associated with them.

    \sa setObjectPickId(), clearPickObjects(), pickObject()
*/
int QGLPainter::objectPickId() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return (d->pick ? d->pick->objectPickId : -1);
}

/*!
    Sets the current object pick identifier to \a value.  If \a value
    is -1, then subsequent objects will be rendered without a pickColor().

    If value is not -1, then the pickColor() is changed to a color
    that represents that object pick identifier.  If \a value has been
    seen previously, then the same pickColor() as last time will
    be returned.

    The function call will be ignored if isPicking() is false.

    \sa objectPickId(), clearPickObjects(), pickObject()
*/
void QGLPainter::setObjectPickId(int value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->pick || !d->pick->isPicking)
        return;
    d->pick->objectPickId = value;
    if (value != -1) {
        QRgb color = d->pick->pickObjectToColor.value(value, 0);
        if (!color) {
            color = qt_qgl_pick_color(d->pick->pickColorIndex++);
            d->pick->pickObjectToColor[value] = color;
            d->pick->pickColorToObject[color] = value;
        }
        d->pick->pickColor = color;
        d->updates |= UpdateColor;
    } else {
        d->pick->pickColor = 0;
        d->updates |= UpdateColor;
    }
}

/*!
    Clears the objectPickId() to pickColor() mappings that
    were used previously.  This will also set objectPickId()
    to -1 and pickColor() to (0, 0, 0, 1).

    The function call will be ignored if isPicking() is false.

    \sa objectPickId(), pickColor()
*/
void QGLPainter::clearPickObjects()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->pick && d->pick->isPicking) {
        d->pick->pickObjectToColor.clear();
        d->pick->pickColorToObject.clear();
        d->pick->pickColorIndex = 0;
        d->pick->objectPickId = -1;
        d->pick->pickColor = 0;
        d->updates |= UpdateColor;
    }
}

/*!
    Returns the current pick color to use to render the object
    associated with objectPickId().  The returned color will
    be (0, 0, 0, 1) if objectPickId() is -1.

    \sa objectPickId(), clearPickObjects()
*/
QColor QGLPainter::pickColor() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->pick) {
        QColor color;
        color.setRgb(d->pick->pickColor);
        return color;
    } else {
        return Qt::black;
    }
}

/*!
    Picks the color at (\a x, \a y) in the color buffer and
    returns the objectPickId() that corresponds to that color.
    Returns -1 if (\a x, \a y) is not positioned over a
    recognized object.  The origin (0, 0) is assumed to be
    the bottom-left corner of the drawing surface.

    \sa objectPickId()
*/
int QGLPainter::pickObject(int x, int y) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();

    if (!d->pick)
    {
        return -1;
    }

    // Fetch the color at the specified pixel.
    unsigned char data[4] = {0, 0, 0, 0};
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    QRgb color = qRgb(data[0], data[1], data[2]);

    // Normalize the color to account for floating-point rounding.
    color = qt_qgl_normalize_pick_color(color); // XXX: detect RGB444 screens.

    // Map the color back to an object identifier.
    return d->pick->pickColorToObject.value(color, -1);
}

QT_END_NAMESPACE
