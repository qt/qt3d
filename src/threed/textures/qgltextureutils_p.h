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

#ifndef QGLTEXTUREUTILS_P_H
#define QGLTEXTUREUTILS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QOpenGLBuffer>
#include <QDateTime>
#include <QSize>

#include "qglnamespace.h"
#include "qopenglfunctions.h"
#include "qgltexture2d.h"

#include <private/qopenglcontext_p.h>

QT_BEGIN_NAMESPACE

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif
#ifndef GL_UNSIGNED_SHORT_5_6_5
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#endif
#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#endif
#ifndef GL_TEXTURE_CUBE_MAP
#define GL_TEXTURE_CUBE_MAP 0x8513
#endif
#ifndef GL_TEXTURE_CUBE_MAP_POSITIVE_X
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#endif
#ifndef GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#endif

#ifndef GL_GENERATE_MIPMAP_SGIS
#define GL_GENERATE_MIPMAP_SGIS       0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS  0x8192
#endif

#if !defined(QT_OPENGL_ES)
#define q_glTexParameteri(target,name,value) \
        glTexParameteri((target), (name), int(value))
#else
#define q_glTexParameteri(target,name,value) \
        glTexParameterf((target), (name), GLfloat(int(value)))
#endif

typedef void (QOPENGLF_APIENTRYP q_glCompressedTexImage2DARB)
    (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);

class QGLTextureExtensions
{
public:
    QGLTextureExtensions(QOpenGLContext *ctx);
    ~QGLTextureExtensions();

    int npotTextures : 1;
    int generateMipmap : 1;
    int bgraTextureFormat : 1;
    int ddsTextureCompression : 1;
    int etc1TextureCompression : 1;
    int pvrtcTextureCompression : 1;
    q_glCompressedTexImage2DARB compressedTexImage2D;

    static QGLTextureExtensions *extensions();
};

class QGLBoundTexture
{
public:
    QGLBoundTexture();
    ~QGLBoundTexture();

    QOpenGLContext *context() const {
        return m_context;
    }

    GLuint textureId() const { return m_resourceId; }
    void setTextureId(QOpenGLContext *ctx, GLuint id)
    {
        m_context = ctx; m_resourceId = id;
    }
    void clearId() { m_resourceId = 0; m_context = 0; }

    QGLTexture2D::BindOptions options() const { return m_options; }
    void setOptions(QGLTexture2D::BindOptions options) { m_options = options; }

    QSize size() const { return m_size; }
    bool hasAlpha() const { return m_hasAlpha; }

    void startUpload(QOpenGLContext *ctx, GLenum target, const QSize &imageSize);
    void uploadFace(GLenum target, const QImage &image, const QSize &scaleSize,
                    GLenum format = GL_RGBA);
    void createFace(GLenum target, const QSize &size, GLenum format = GL_RGBA);
    void finishUpload(GLenum target);

    static bool canBindCompressedTexture
        (const char *buf, int len, const char *format, bool *hasAlpha,
         bool *isFlipped);
    bool bindCompressedTexture
        (const QString& fileName, const char *format = 0);
    bool bindCompressedTexture
        (const char *buf, int len, const char *format = 0);
    bool bindCompressedTextureDDS(const char *buf, int len);
    bool bindCompressedTexturePVR(const char *buf, int len);

private:
    QGLTexture2D::BindOptions m_options;
    QSize m_size;
    bool m_hasAlpha;
    QTime time;
    QOpenGLContext *m_context;
    GLuint m_resourceId;
};

QT_END_NAMESPACE

#endif
