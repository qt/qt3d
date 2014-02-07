/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qglext_p.h"
#include <qopenglfunctions.h>

#include <private/qopenglcontext_p.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_OPENGL_ES)

typedef void (QOPENGLF_APIENTRYP q_PFNGLCLIENTACTIVETEXTUREPROC) (GLenum);

class QGLMultiTextureExtensions
{
public:
    QGLMultiTextureExtensions() :
        clientActiveTexture(0),
        multiTextureResolved(false)
    {
    }
    ~QGLMultiTextureExtensions();

    void invalidateExtension();

    q_PFNGLCLIENTACTIVETEXTUREPROC clientActiveTexture;
    bool multiTextureResolved;
};


static QGLMultiTextureExtensions *qt_multitexture_funcs = 0;

QGLMultiTextureExtensions::~QGLMultiTextureExtensions()
{
    qt_multitexture_funcs = 0;
}

void QGLMultiTextureExtensions::invalidateExtension()
{
    qt_multitexture_funcs = 0;
}

static QGLMultiTextureExtensions *resolveMultiTextureExtensions
    (const QOpenGLContext *ctx)
{
    Q_ASSERT(ctx);
    if (!qt_multitexture_funcs)
        qt_multitexture_funcs = new QGLMultiTextureExtensions();
    if (!(qt_multitexture_funcs->multiTextureResolved)) {
        qt_multitexture_funcs->multiTextureResolved = true;
        if (!qt_multitexture_funcs->clientActiveTexture) {
            QOpenGLContext *vctx = const_cast<QOpenGLContext*>(ctx);
            qt_multitexture_funcs->clientActiveTexture = (q_PFNGLCLIENTACTIVETEXTUREPROC)
                vctx->getProcAddress("glClientActiveTexture");
        }
        if (!qt_multitexture_funcs->clientActiveTexture) {
            QOpenGLContext *vctx = const_cast<QOpenGLContext*>(ctx);
            qt_multitexture_funcs->clientActiveTexture = (q_PFNGLCLIENTACTIVETEXTUREPROC)
                vctx->getProcAddress("glClientActiveTextureARB");
        }
    }
    return qt_multitexture_funcs;
}

void qt_gl_ClientActiveTexture(GLenum texture)
{
    const QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx)
        return;
    QGLMultiTextureExtensions *extn = resolveMultiTextureExtensions(ctx);
    if (extn->clientActiveTexture)
        extn->clientActiveTexture(texture);
}

#endif

QT_END_NAMESPACE
