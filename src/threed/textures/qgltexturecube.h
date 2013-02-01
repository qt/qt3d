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

#ifndef QGLTEXTURECUBEMAP_H
#define QGLTEXTURECUBEMAP_H

#include <Qt3D/qglnamespace.h>
#include <Qt3D/qgltexture2d.h>

#include <QOpenGLBuffer>

QT_BEGIN_NAMESPACE

class QGLTextureCubePrivate;

class Q_QT3D_EXPORT QGLTextureCube
{
public:
    QGLTextureCube();
    ~QGLTextureCube();

    enum Face
    {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    };

    bool isNull() const;
    bool hasAlphaChannel() const;

    QSize size() const;
    void setSize(const QSize& value);
    QSize requestedSize() const;

    QImage image(QGLTextureCube::Face face) const;
    void setImage(QGLTextureCube::Face face, const QImage& image);
    void clearImage(QGLTextureCube::Face face);

    QGLTexture2D::BindOptions bindOptions() const;
    void setBindOptions(QGLTexture2D::BindOptions options);

    QGL::TextureWrap horizontalWrap() const;
    void setHorizontalWrap(QGL::TextureWrap value);

    QGL::TextureWrap verticalWrap() const;
    void setVerticalWrap(QGL::TextureWrap value);

    bool cleanupResources();
    bool bind() const;
    static void release();

    GLuint textureId() const;

    static QGLTextureCube *fromTextureId(GLuint id, const QSize& size);

private:
    QScopedPointer<QGLTextureCubePrivate> d_ptr;

    Q_DISABLE_COPY(QGLTextureCube)
    Q_DECLARE_PRIVATE(QGLTextureCube)
};

QT_END_NAMESPACE

#endif
