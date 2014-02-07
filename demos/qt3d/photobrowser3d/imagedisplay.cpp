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

#include "imagedisplay.h"
#include "thumbnailableimage.h"
#include "thumbnailnode.h"
#include "thumbnaileffect.h"
#include "qglbuilder.h"
#include "qglcube.h"
#include "imagemanager.h"
#include "qatlas.h"
#include "qglshaderprogrameffect.h"
#include "qphotobrowser3dscene.h"
#include "photobrowser3dview.h"

#include <QGuiApplication>
#include <QChildEvent>
#include <QCloseEvent>
#include <QUrl>
#include <QImage>

static inline QImage qMakeFrameImage()
{
    QImage frm(QSize(128, 128), QImage::Format_ARGB32);
    frm.fill(qRgba(8, 8, 8, 255));  // dark grey frame
    QPainter ptr;
    ptr.begin(&frm);
    QRect r(8, 8, 112, 112);
    ptr.setBackgroundMode(Qt::TransparentMode);
    ptr.fillRect(r, QColor(0, 30, 50, 64));
    ptr.setPen(QColor("orange"));
    ptr.drawText(frm.rect(), Qt::AlignCenter, "Loading...");
    ptr.end();
    return frm;
}

static inline void qAddPane(QSizeF size, QGeometryData *data)
{
    Q_ASSERT(data);
    QSizeF f = size / 2.0f;
    QVector2D a(-f.width(), -f.height());
    QVector2D b(f.width(), -f.height());
    QVector2D c(f.width(), f.height());
    QVector2D d(-f.width(), f.height());
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    QVector2D td(0.0f, 1.0f);
    int k = data->count();
    data->appendVertex(a, b, c, d);
    data->appendTexCoord(ta, tb, tc, td);
    data->appendIndices(k, k+1, k+2);
    data->appendIndices(k, k+2, k+3);
}

ImageDisplay::ImageDisplay(QObject *parent, QSharedPointer<QGLMaterialCollection> materials, float wallSize)
    : QGLSceneNode(parent)
    , m_wall(0)
    , m_frames(0)
    , m_currentWall(0)
    , m_currentFrame(0)
    , m_imageSetToDefault(false)
    , m_count(0)
    , m_size(wallSize)
    , m_frameSize((m_size * 3.0f) / 4.0f)
    , m_maxImages(500)
    , m_frameLoadingMaterial(-1)
{
    // the real values will get poked in here by the atlas
    m_atlasPlaceHolder.append(QVector2D(), QVector2D(), QVector2D(), QVector2D());

    setObjectName("ImageDisplay");
    setPalette(materials);

    // the frames lie in Z = 0, the wall is set back and lies in
    // the plane Z = (m_size / -4.0)

    // build the wall
    qAddPane(QSize(m_size, m_size), &m_wallGeometry);
    m_wall = new QGLSceneNode(this);
    m_wall->setObjectName("Wall");
    m_wall->setPalette(materials);
    m_currentWall = new QGLSceneNode(m_wall);
    m_currentWall->setObjectName("wall 0");
    m_currentWall->setGeometry(m_wallGeometry);
    m_currentWall->setCount(m_wallGeometry.indexCount());
    m_wall->setPosition(QVector3D(0.0f, 0.0f, m_size / -4.0));

    // paint the wall
    m_wall->setEffect(QGL::FlatReplaceTexture2D);
    QGLMaterial *mat = new QGLMaterial();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    m_LoadedTextures.push_back(tex);
    tex->setImage(QImage(":/res/images/girder.png"));
    mat->setTexture(tex);
    mat->setObjectName("girder material");
    m_wall->setMaterial(mat);

    // build the frames
    qAddPane(QSize(m_frameSize, m_frameSize), &m_frameGeometry);
    m_frameGeometry.appendTexCoordArray(m_atlasPlaceHolder, QGL::TextureCoord1);
    m_frames = new QGLSceneNode(this);
    m_frames->setObjectName("Frames");
    m_currentFrame = new ThumbnailNode(m_frames);
    m_currentFrame->setObjectName("frame 0");
    m_currentFrame->setGeometry(m_frameGeometry);
    m_currentFrame->setCount(m_frameGeometry.indexCount());

    // use the frames geometry to put the atlas data into
    QAtlas *atlas = QAtlas::instance();
    atlas->setGeometry(m_frameGeometry);

    // generally the frames use the thumbnail material & effect
    m_effect = new ThumbnailEffect;
    m_frames->setUserEffect(m_effect);
    m_frames->setEffectEnabled(true);
    m_frames->setMaterial(atlas->material());

    // unless they're loading, in which case use the "loading" image
    m_frameImage = qMakeFrameImage();
    mat = new QGLMaterial();
    tex = new QGLTexture2D(mat);
    m_LoadedTextures.push_back(tex);
    tex->setHorizontalWrap(QGL::ClampToEdge);
    tex->setImage(m_frameImage);
    mat->setTexture(tex);
    mat->setObjectName("loading image material - default");
    m_frameLoadingMaterial = materials->addMaterial(mat);
    m_currentFrame->setMaterialIndex(m_frameLoadingMaterial);

    // make the frames pickable
    PhotoBrowser3DView *view = qobject_cast<PhotoBrowser3DView*>(parent);
    view->scene()->mainNode()->addNode(m_frames);

    m_imageSetToDefault = true;
}

ImageDisplay::~ImageDisplay()
{
    for (int i=0; i<m_LoadedTextures.count(); ++i) {
        m_LoadedTextures.at(i)->cleanupResources();
    }
    delete m_effect;
}

void ImageDisplay::addThumbnailNode(const QUrl &image)
{
    Q_ASSERT(QThread::currentThread() == thread());
    ImageManager *manager = qobject_cast<ImageManager*>(sender());
    if (!m_imageSetToDefault)
    {
        QVector3D p = m_currentFrame->position();
        p.setX(p.x() - m_size);
        int start = m_frameGeometry.indexCount();
        qAddPane(QSize(m_frameSize, m_frameSize), &m_frameGeometry);
        int count = m_frameGeometry.indexCount() - start;
        m_frameGeometry.appendTexCoordArray(m_atlasPlaceHolder, QGL::TextureCoord1);
        m_currentFrame = new ThumbnailNode(m_frames);
        QString name = QLatin1String("frame %1");
        name.arg(m_count);
        m_currentFrame->setObjectName(name);
        m_currentFrame->setPosition(p);
        m_currentFrame->setStart(start);
        m_currentFrame->setCount(count);
        m_currentFrame->setGeometry(m_frameGeometry);
        m_currentFrame->setMaterialIndex(m_frameLoadingMaterial);

        QGLSceneNode *s = m_currentWall->clone(m_wall);
        name = QLatin1String("wall %1");
        name.arg(m_count);
        s->setObjectName(name);
        p = s->position();
        p.setX(p.x() - m_size);
        s->setPosition(p);
        m_currentWall = s;
    }
    m_currentFrame->setUrl(image);
    if (manager)
    {
        connect(m_currentFrame, SIGNAL(imageRequired(ThumbnailableImage)),
                manager, SIGNAL(deployLoader(ThumbnailableImage)));
        connect(manager, SIGNAL(imageReady(ThumbnailableImage)),
                m_currentFrame, SLOT(setImage(ThumbnailableImage)));
    }
    PhotoBrowser3DView *view = qobject_cast<PhotoBrowser3DView*>(parent());
    Q_ASSERT(view);
    connect(m_currentFrame, SIGNAL(nodeChanged()), view, SLOT(update()));

    m_imageSetToDefault = false;
    emit framesChanged();
    ++m_count;
}
