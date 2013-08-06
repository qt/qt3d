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

#ifndef QGLTEXTURE2D_P_H
#define QGLTEXTURE2D_P_H

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

#include "qgltexture2d.h"
#include "qgltextureutils_p.h"
#include "qurl.h"
#include "qdownloadmanager.h"

#include <QDebug>
#include <QAtomicInt>
#include <QImage>

QT_BEGIN_NAMESPACE

class QGLTexture2DTextureInfo
{
public:
    QGLTexture2DTextureInfo
            (QOpenGLContext *context, GLuint textureId, uint _imageGeneration,
             uint _parameterGeneration, bool _isLiteral = false)
        : imageGeneration(_imageGeneration)
        , parameterGeneration(_parameterGeneration)
        , isLiteral(_isLiteral)
    {
        if (textureId)
            tex.setTextureId(context, textureId);
    }

    QGLBoundTexture tex;
    uint imageGeneration;
    uint parameterGeneration;
    bool isLiteral;
};

class DDSFormat;

class QGLTexture2DPrivate
{
public:
    QGLTexture2DPrivate();
    virtual ~QGLTexture2DPrivate();

    QSize size;
    QSize requestedSize;
    QImage image;
    QUrl url;
    QByteArray compressedData;
    QGLTexture2D::BindOptions bindOptions;
    QGL::TextureWrap horizontalWrap;
    QGL::TextureWrap verticalWrap;
#if !defined(QT_OPENGL_ES)
    bool mipmapSupported;
    bool mipmapSupportedKnown;
#endif
    uint imageGeneration;
    uint parameterGeneration;
    QList<QGLTexture2DTextureInfo*>  textureInfo;
    bool sizeAdjusted;
    QDownloadManager *downloadManager;
    bool bind(GLenum target);
    virtual void bindImages(QGLTexture2DTextureInfo *info);
    void adjustForNPOTTextureSize();

    bool cleanupResources();
};

typedef QMap<QOpenGLContext*,QList<GLuint> > PendingResourcesMap;
typedef PendingResourcesMap::iterator PendingResourcesMapIter;
class QToBeDeleted : public QObject
{
    Q_OBJECT
public:
    QToBeDeleted(QObject *parent = 0);
    ~QToBeDeleted();

    PendingResourcesMap m_ToBeDeleted;

    void processPendingResourceDeallocations();

public Q_SLOTS:
    void mournGLContextDeath();
};

QT_END_NAMESPACE

#endif
