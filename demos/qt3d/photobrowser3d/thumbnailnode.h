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
    void setThreshold(qreal threshold) { m_thresholdSquared = threshold * threshold; }
    qreal threshold() const { return qSqrt(m_thresholdSquared); }
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
    qreal m_thresholdSquared;
    int m_defaultMaterial;
    QUrl m_url;
    bool m_loading;
    QGLSceneNode *m_full;
    QSizeF m_max;
    QObject *m_manager;
    Distance m_lastDistance;
};

#endif // THUMBNAILNODE_H
