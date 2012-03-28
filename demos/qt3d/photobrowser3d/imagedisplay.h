/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include "qglscenenode.h"
#include "qglmaterialcollection.h"

#include <QString>

QT_BEGIN_NAMESPACE
class QGLBuilder;
class QGLTexture2D;
class QGLPainter;
QT_END_NAMESPACE

class ThumbnailNode;

class ImageDisplay : public QGLSceneNode
{
    Q_OBJECT
public:
    ImageDisplay(QObject *parent, QGLMaterialCollection *materials, qreal wallSize = 4.0);
    ~ImageDisplay();
    int maxImages() const { return m_maxImages; }
    void setMaxImages(int max) { m_maxImages = max; }
signals:
    void framesChanged();
public slots:
    void addThumbnailNode(const QUrl &url);
private:
    QGLSceneNode *m_wall;
    QGLSceneNode *m_frames;
    QGLSceneNode *m_currentWall;
    ThumbnailNode *m_currentFrame;
    QGLAbstractEffect *m_effect;
    bool m_imageSetToDefault;
    int m_count;
    qreal m_size;
    qreal m_frameSize;
    int m_maxImages;
    QImage m_frameImage;
    int m_frameLoadingMaterial;
    QGeometryData m_frameGeometry;
    QGeometryData m_wallGeometry;
    QVector2DArray m_atlasPlaceHolder;
};

#endif // IMAGEDISPLAY_H
