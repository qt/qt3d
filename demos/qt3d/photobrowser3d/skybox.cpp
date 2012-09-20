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

#include "skybox.h"
#include "qglbuilder.h"
#include "qglcube.h"
#include "qglpainter.h"
#include "qglview.h"

#include <QDir>
#include <QFileInfo>

SkyBox::SkyBox(QGLView *view, const QString &imagePath)
    : QObject(view)
    , m_scene(0)
    , m_view(view)
    , m_camera(new QGLCamera(this))
{
    m_camera->setEye(QVector3D());
    m_camera->setNearPlane(0.3f);
    m_camera->setViewSize(QSizeF(0.3f, 0.3f));

    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    QVector3D blb(-1.0, -1.0, -1.0);
    QVector3D blf(-1.0, -1.0, 1.0);
    QVector3D tlf(-1.0, 1.0, 1.0);
    QVector3D tlb(-1.0, 1.0, -1.0);
    QVector3D brb(1.0, -1.0, -1.0);
    QVector3D brf(1.0, -1.0, 1.0);
    QVector3D trf(1.0, 1.0, 1.0);
    QVector3D trb(1.0, 1.0, -1.0);
    QVector2D bl(0.0f, 0.0f);
    QVector2D br(1.0f, 0.0f);
    QVector2D tr(1.0f, 1.0f);
    QVector2D tl(0.0f, 1.0f);
    {
        QGeometryData q;   // left
        builder.currentNode()->setObjectName("left");
        q.appendVertex(blf, blb, tlb, tlf);
        q.appendTexCoord(bl, br, tr, tl);
        builder.addQuads(q);
        m_faces[0] = builder.currentNode();
        m_faces[0]->setMaterial(new QGLMaterial);
    }
    {
        builder.newNode();   // top
        builder.currentNode()->setObjectName("top");
        QGeometryData q;
        q.appendVertex(trf, tlf, tlb, trb);
        q.appendTexCoord(bl, br, tr, tl);
        builder.addQuads(q);
        m_faces[1] = builder.currentNode();
        m_faces[1]->setMaterial(new QGLMaterial);
    }
    {
        builder.newNode();    // right
        builder.currentNode()->setObjectName("right");
        QGeometryData q;
        q.appendVertex(brb, brf, trf, trb);
        q.appendTexCoord(bl, br, tr, tl);
        builder.addQuads(q);
        m_faces[2] = builder.currentNode();
        m_faces[2]->setMaterial(new QGLMaterial);
    }
    {
        builder.newNode();    // bottom
        builder.currentNode()->setObjectName("bottom");
        QGeometryData q;
        q.appendVertex(brb, blb, blf, brf);
        q.appendTexCoord(bl, br, tr, tl);
        builder.addQuads(q);
        m_faces[3] = builder.currentNode();
        m_faces[3]->setMaterial(new QGLMaterial);
    }
    {
        builder.newNode();    // front
        builder.currentNode()->setObjectName("front");
        QGeometryData q;
        q.appendVertex(brf, blf, tlf, trf);
        q.appendTexCoord(bl, br, tr, tl);
        builder.addQuads(q);
        m_faces[4] = builder.currentNode();
        m_faces[4]->setMaterial(new QGLMaterial);
    }
    {
        builder.newNode();    // back
        builder.currentNode()->setObjectName("back");
        QGeometryData q;
        q.appendVertex(blb, brb, trb, tlb);
        q.appendTexCoord(bl, br, tr, tl);
        builder.addQuads(q);
        m_faces[5] = builder.currentNode();
        m_faces[5]->setMaterial(new QGLMaterial);
    }
    m_scene = builder.finalizedSceneNode();
    m_scene->setObjectName("SkyboxList");
    m_scene->setEffect(QGL::FlatReplaceTexture2D);
    m_scene->setEffectEnabled(true);
    m_scene->setParent(this);

    QString resourceBase = QLatin1String(":/");
    setImagePath(imagePath.isEmpty() ? resourceBase : imagePath);
}

SkyBox::~SkyBox()
{
    for (int i=0; i<6; ++i) {
        m_faces[i]->material()->texture()->cleanupResources();
    }
}

void SkyBox::setImagePath(const QString &imagePath)
{
    static QStringList expected2;
    static QStringList expected;

    if (expected.isEmpty())
        expected << QLatin1String("east") << QLatin1String("up") << QLatin1String("west")
                 << QLatin1String("down") << QLatin1String("south") << QLatin1String("north");
    if (expected2.isEmpty())
        expected2 << QLatin1String("right") << QLatin1String("top") << QLatin1String("left")
                  << QLatin1String("bottom") << QLatin1String("front") << QLatin1String("back");
    if (imagePath != m_imagePath)
    {
        m_imagePath = imagePath;
        QStringList notFound = expected;
        QFileInfo info(m_imagePath);
        if (info.exists() && info.isDir())
        {
            QDir imDir(imagePath);
            QFileInfoList files = imDir.entryInfoList(QDir::Files);
            QFileInfoList::const_iterator it = files.constBegin();
            for ( ; it != files.constEnd() && notFound.size() > 0; ++it)
            {
                QFileInfo ent = *it;
                QString tok = ent.baseName().toLower();
                int ix = 0;
                for ( ; ix < 6; ++ix)
                    if (tok.contains(expected.at(ix)))
                        break;
                if (ix == 6)
                {
                    ix = 0;
                    for ( ; ix < 6; ++ix)
                        if (tok.contains(expected2.at(ix)))
                            break;
                }
                if (ix != 6)
                {
                    notFound.removeOne(expected.at(ix));
                    QUrl url;
                    url.setScheme("file");
                    url.setPath(ent.absoluteFilePath());
                    m_faces[ix]->material()->setTextureUrl(url);
                    m_faces[ix]->material()->texture()->setHorizontalWrap(QGL::ClampToEdge);
                    m_faces[ix]->material()->texture()->setVerticalWrap(QGL::ClampToEdge);
                }
            }
            if (notFound.size() > 2)
            {
                qWarning("Could not load textures for");
                for (int i = 0; i < notFound.size(); ++i)
                    qWarning("\t%s", qPrintable(notFound.at(i)));
            }
        }
        else
        {
            qWarning("SkyBox::setImagePath: Not an existing directory %s",
                     qPrintable(m_imagePath));
        }
    }
}

void SkyBox::draw(QGLPainter *painter) const
{
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().setToIdentity();

    QGLCamera *cam = m_view->camera();
    m_camera->setCenter(-cam->eye());
    painter->setCamera(m_camera);

    glDisable(GL_DEPTH_TEST);

    m_scene->draw(painter);

    glEnable(GL_DEPTH_TEST);

    painter->setCamera(cam);
    painter->modelViewMatrix().pop();
}
