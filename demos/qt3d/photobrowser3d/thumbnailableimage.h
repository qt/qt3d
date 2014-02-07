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

#ifndef THUMBNAILABLEIMAGE_H
#define THUMBNAILABLEIMAGE_H

#include <QRectF>
#include <QMetaType>

#include "qarray.h"
#include "qgeometrydata.h"

QT_BEGIN_NAMESPACE
class QImage;
class QGLPainter;
QT_END_NAMESPACE

class ThumbnailableImagePrivate;

class ThumbnailableImage
{
public:
    ThumbnailableImage();
    ThumbnailableImage(const ThumbnailableImage&);
    ~ThumbnailableImage();

    ThumbnailableImage &operator=(const ThumbnailableImage &);

    void setThumbnailed(bool enable);
    bool isThumbnailed() const;

    QImage data() const;
    void setData(QImage data);

    QUrl url() const;
    void setUrl(const QUrl &url);

    QRectF frame() const;
    float scale() const;

    void minimize();
    bool isMinimized() const;

    QGL::IndexArray indices() const;
    void setIndices(const QGL::IndexArray &indices);

    bool isNull() const { return d  == 0; }

    ThumbnailableImagePrivate *priv() const { return d; } /// debug = remove me

private:
    void detach();

    ThumbnailableImagePrivate *d;
};

Q_DECLARE_METATYPE(ThumbnailableImage);

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const ThumbnailableImage &image);
#endif

#endif // THUMBNAILABLEIMAGE_H
