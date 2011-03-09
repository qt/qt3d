/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsgstereocontext.h"
#include "qsgpretransformnode_p.h"
#include "qglnamespace.h"
#include <QtDeclarative/private/nodeupdater_p.h>
#include <QtDeclarative/renderer.h>
#include <QtOpenGL/qglframebufferobject.h>

QT_BEGIN_NAMESPACE

// Determine how to switch drawing buffers.  On desktop OpenGL we
// use glDrawBuffer().  On OpenGL/ES we attempt to resolve either
// glDrawBufferOES() or glDrawBuffersOES(), under the assumption
// that the OpenGL/ES implementation has a buffer-switching extension
// that mirrors how desktop OpenGL works.  For other kinds of buffer
// switching, modify QSGStereoContext::renderNextFrame().
#if defined(GL_BACK_LEFT) && defined(GL_BACK_RIGHT) && !defined(QT_OPENGL_ES)
    #define DESKTOP_GL_DRAW_BUFFERS 1
#elif defined(QT_OPENGL_ES)
    #define OPENGL_ES_DRAW_BUFFERS 1
#endif
#ifndef GL_BACK_LEFT
#define GL_BACK_LEFT 0x0402
#endif
#ifndef GL_BACK_RIGHT
#define GL_BACK_RIGHT 0x0403
#endif

#ifdef Q_WS_WIN
#    define QSG_GLF_APIENTRY APIENTRY
#endif
#ifndef Q_WS_MAC
# ifndef QSG_GLF_APIENTRYP
#   ifdef QSG_GLF_APIENTRY
#     define QSG_GLF_APIENTRYP QSG_GLF_APIENTRY *
#   else
#     define QSG_GLF_APIENTRY
#     define QSG_GLF_APIENTRYP *
#   endif
# endif
#else
# define QSG_GLF_APIENTRY
# define QSG_GLF_APIENTRYP *
#endif

typedef void (QSG_GLF_APIENTRYP q_PFNGLDRAWBUFFERPROC)(GLenum mode);
typedef void (QSG_GLF_APIENTRYP q_PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum *bufs);

class QSGStereoContextPrivate
{
public:
    enum StereoMode
    {
        Autodetect,
        HardwareStereo,
        RedCyanStereo
    };

    QSGStereoContextPrivate()
        : eye(QGL::NoEye)
        , stereoMode(QSGStereoContextPrivate::Autodetect)
#if defined(OPENGL_ES_DRAW_BUFFERS)
        , functionsResolved(false)
        , drawBuffer(0)
        , drawBuffers(0)
#endif
    {
    }

    QGL::Eye eye;
    StereoMode stereoMode;

#if defined(OPENGL_ES_DRAW_BUFFERS)
    bool functionsResolved;
    q_PFNGLDRAWBUFFERPROC drawBuffer;
    q_PFNGLDRAWBUFFERSPROC drawBuffers;
#endif

    void setEye(QGL::Eye value) { eye = value; }
    void setDrawBuffer(GLenum buffer);
};

void QSGStereoContextPrivate::setDrawBuffer(GLenum buffer)
{
#if defined(DESKTOP_GL_DRAW_BUFFERS)
    glDrawBuffer(buffer);
#elif defined(OPENGL_ES_DRAW_BUFFERS)
    if (!functionsResolved) {
        QGLContext *context = const_cast<QGLContext *>(QGLContext::currentContext());
        drawBuffer = (q_PFNGLDRAWBUFFERPROC)
            context->getProcAddress(QLatin1String("glDrawBuffer"));
        if (!drawBuffer) {
            drawBuffer = (q_PFNGLDRAWBUFFERPROC)
                context->getProcAddress(QLatin1String("glDrawBufferOES"));
        }
        if (!drawBuffer) {
            drawBuffer = (q_PFNGLDRAWBUFFERPROC)
                context->getProcAddress(QLatin1String("glDrawBufferARB"));
        }
        drawBuffers = (q_PFNGLDRAWBUFFERSPROC)
            context->getProcAddress(QLatin1String("glDrawBuffers"));
        if (!drawBuffers) {
            drawBuffers = (q_PFNGLDRAWBUFFERSPROC)
                context->getProcAddress(QLatin1String("glDrawBuffersOES"));
        }
        if (!drawBuffers) {
            drawBuffers = (q_PFNGLDRAWBUFFERSPROC)
                context->getProcAddress(QLatin1String("glDrawBuffersARB"));
        }
        if (!drawBuffer && !drawBuffers) {
            qWarning("QSGContext: QGLContext is stereo-capable but "
                     "glDrawBuffersOES() is not available");
        }
        functionsResolved = true;
    }
    if (drawBuffer)
        drawBuffer(buffer);
    else if (drawBuffers)
        drawBuffers(1, &buffer);
#else
    Q_UNUSED(buffer);
#endif
}

class CurrentContextBindable : public Bindable
{
public:
    CurrentContextBindable()
        : m_ctx(const_cast<QGLContext *>(QGLContext::currentContext()))
    {
    }

    void bind() const;

private:
    QGLContext *m_ctx;
};

class DrawBufferBindable : public CurrentContextBindable
{
public:
    DrawBufferBindable(QSGStereoContextPrivate *context, GLenum buffer)
        : m_context(context), m_buffer(buffer) {}

    void bind() const;

private:
    QSGStereoContextPrivate *m_context;
    GLenum m_buffer;
};

class RedStereoBindable : public CurrentContextBindable
{
public:
    void clear() const;
    void reactivate() const;
};

class CyanStereoBindable : public CurrentContextBindable
{
public:
    void clear() const;
    void reactivate() const;
};

void CurrentContextBindable::bind() const
{
    m_ctx->makeCurrent();
    QGLFramebufferObject::bindDefault();
}

void DrawBufferBindable::bind() const
{
    CurrentContextBindable::bind();
    m_context->setDrawBuffer(m_buffer);
}

void RedStereoBindable::clear() const
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
}

void RedStereoBindable::reactivate() const
{
    glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
}

void CyanStereoBindable::clear() const
{
    // Only need to clear the depth buffer on the second pass.
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE);
}

void CyanStereoBindable::reactivate() const
{
    glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE);
}

class QSGStereoNodeUpdater : public NodeUpdater
{
protected:
    void enterPreTransformNode(QSGPreTransformNode *);
    void leavePreTransformNode(QSGPreTransformNode *);

    void visitNode(Node *n);
};

void QSGStereoNodeUpdater::enterPreTransformNode(QSGPreTransformNode *t)
{
    if (t->dirtyFlags() & Node::DirtyMatrix)
        ++m_force_update;

    if (!t->preMatrix().isIdentity() || !t->matrix().isIdentity()) {
        m_combined_matrix_stack.push(&t->combinedMatrix());

        m_matrix_stack.push();
        m_matrix_stack = t->preMatrix() * m_matrix_stack.top() * t->matrix();
    }

    t->setCombinedMatrix(m_matrix_stack.top());
}

void QSGStereoNodeUpdater::leavePreTransformNode(QSGPreTransformNode *t)
{
    if (t->dirtyFlags() & Node::DirtyMatrix)
        --m_force_update;

    if (!t->preMatrix().isIdentity() || !t->matrix().isIdentity()) {
        m_matrix_stack.pop();
        m_combined_matrix_stack.pop();
    }
}

void QSGStereoNodeUpdater::visitNode(Node *n)
{
    if (n->type() == QSGPreTransformNode::PreTransformNodeType) {
        QSGPreTransformNode *t = static_cast<QSGPreTransformNode *>(n);
        enterPreTransformNode(t);
        visitChildren(t);
        leavePreTransformNode(t);
    } else {
        NodeUpdater::visitNode(n);
    }
}

QSGStereoContext::QSGStereoContext(const QString &key, QObject *parent)
    : QSGContext(parent)
    , d_ptr(new QSGStereoContextPrivate)
{
    Q_D(QSGStereoContext);
    if (key == QLatin1String("stereo-test"))
        d->stereoMode = QSGStereoContextPrivate::RedCyanStereo;
}

QSGStereoContext::~QSGStereoContext()
{
}

int QSGStereoContext::eye() const
{
    Q_D(const QSGStereoContext);
    return int(d->eye);
}

Renderer *QSGStereoContext::createRenderer()
{
    Renderer *renderer = QSGContext::createRenderer();
    renderer->setNodeUpdater(new QSGStereoNodeUpdater);
    return renderer;
}

void QSGStereoContext::renderNextFrame()
{
    Q_D(QSGStereoContext);

    emit aboutToRenderNextFrame();

    // Detect the type of stereo we should use the first time we render.
    if (d->stereoMode == QSGStereoContextPrivate::Autodetect) {
        if (glContext()->format().stereo())
            d->stereoMode = QSGStereoContextPrivate::HardwareStereo;
        else
            d->stereoMode = QSGStereoContextPrivate::RedCyanStereo;
    }

    // Render the scene according to the stereo mode.
    Renderer *renderer = this->renderer();
    switch (d->stereoMode) {
    case QSGStereoContextPrivate::HardwareStereo: {
        DrawBufferBindable left(d, GL_BACK_LEFT);
        DrawBufferBindable right(d, GL_BACK_RIGHT);
        d->setEye(QGL::LeftEye);
        renderer->renderScene(left);
        d->setEye(QGL::RightEye);
        renderer->renderScene(right);
        d->setEye(QGL::NoEye);
        break; }

    case QSGStereoContextPrivate::RedCyanStereo: {
        RedStereoBindable left;
        CyanStereoBindable right;
        d->setEye(QGL::LeftEye);
        renderer->renderScene(left);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        d->setEye(QGL::RightEye);
        renderer->renderScene(right);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        d->setEye(QGL::NoEye);
        break; }

    default: break;
    }
}

QT_END_NAMESPACE
