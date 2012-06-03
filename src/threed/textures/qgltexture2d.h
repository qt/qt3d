/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLTEXTURE2D_H
#define QGLTEXTURE2D_H

#include "qglnamespace.h"

#include <QOpenGLBuffer>
#include <QScopedPointer>
#include <QPoint>
#include <QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLTexture2DPrivate;
class QOpenGLContext;

class Q_QT3D_EXPORT QGLTexture2D : public QObject
{
    Q_OBJECT
public:
    enum BindOption
    {
        NoBindOption                            = 0x0000,
        InvertedYBindOption                     = 0x0001,
        MipmapBindOption                        = 0x0002,
        PremultipliedAlphaBindOption            = 0x0004,
        LinearFilteringBindOption               = 0x0008,
        DefaultBindOption                       =  LinearFilteringBindOption
                                                    | InvertedYBindOption | MipmapBindOption
    };
    Q_DECLARE_FLAGS(BindOptions, BindOption)

    QGLTexture2D(QObject *parent = 0);
    ~QGLTexture2D();

    bool isNull() const;
    bool hasAlphaChannel() const;

    QSize size() const;
    void setSize(const QSize& value);
    QSize requestedSize() const;

    QImage image() const;
    void setImage(const QImage& image);
    bool setCompressedFile(const QString &path);
    QUrl url() const;
    void setUrl(const QUrl &url);

    void setPixmap(const QPixmap& pixmap);

    void clearImage();

    BindOptions bindOptions() const;
    void setBindOptions(BindOptions options);

    QGL::TextureWrap horizontalWrap() const;
    void setHorizontalWrap(QGL::TextureWrap value);

    QGL::TextureWrap verticalWrap() const;
    void setVerticalWrap(QGL::TextureWrap value);

    bool bind() const;
    void release() const;

    GLuint textureId() const;

    bool cleanupResources();

    static QGLTexture2D *fromTextureId(GLuint id, const QSize& size);

    static void toBeDeletedLater(QOpenGLContext*, GLuint);
    static void processPendingResourceDeallocations();

signals:
    void textureUpdated();
public slots:
    void textureRequestFinished(QByteArray*);
private:
    QScopedPointer<QGLTexture2DPrivate> d_ptr;

    Q_DISABLE_COPY(QGLTexture2D)
    Q_DECLARE_PRIVATE(QGLTexture2D)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLTexture2D::BindOptions)

QT_END_NAMESPACE

QT_END_HEADER

#endif
