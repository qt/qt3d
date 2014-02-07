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

#ifndef THUMBNAILNODE_H
#define THUMBNAILNODE_H

#include "qglscenenode.h"
#include "thumbnailableimage.h"

#include <QtCore/qmath.h>
#include <QUrl>

class ThumbnailNode : public QGLSceneNode
{
    Q_OBJECT
public:
    enum Distance
    {
        Unknown,
        Near,
        Middle,
        Far,
        VeryFar
    };

    explicit ThumbnailNode(QObject *parent = 0);
    ~ThumbnailNode();
    QUrl url() const { return m_url; }
    void setUrl(const QUrl &url);
    void setThreshold(float threshold) { m_thresholdSquared = threshold * threshold; }
    float threshold() const { return qSqrt(m_thresholdSquared); }
    void draw(QGLPainter *painter);
    void drawGeometry(QGLPainter *painter);
    ThumbnailableImage image() const { return m_image; }
signals:
    void imageRequired(const ThumbnailableImage &);
    void nodeChanged();
public slots:
    void setImage(const ThumbnailableImage &image);
private:
    void createFullNode();
    void destroyFullNode();
    void setupLoading();
    void setupThumbnailing();
    void loadFullImage();

    ThumbnailableImage m_image;
    float m_thresholdSquared;
    int m_defaultMaterial;
    QUrl m_url;
    bool m_loading;
    QGLSceneNode *m_full;
    QSizeF m_max;
    QObject *m_manager;
    Distance m_lastDistance;

    QList<QGLTexture2D*> m_LoadedTextures;
};

#endif // THUMBNAILNODE_H
