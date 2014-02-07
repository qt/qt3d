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
#include "viewport.h"

#include <QDir>
#include <QFileInfo>

/*!
    \qmltype Skybox
    \instantiates Skybox
    \brief The Skybox item provides a simple 6-texture environment.
    \since 4.8
    \ingroup qt3d::qml3d

    The six textures of the Skybox completely encloses the camera,
    giving the illusion of a full 3D environment.  Although the illusion
    is far from perfect it is adequate for many simple purposes.  It is
    most suited to sky, clouds, remote landscapes (such as mountains) or
    possibly city skylines - anything where the textures represent an
    environment that is effectively infinitely distant from the camera.

    To use a skybox create a directory with six appropriately named images
    and place this alongside the main qml file, then specify the directory
    name for the \l{source} property:

    \code
    Viewport {

        Skybox {
            source: "skybox"
        }

        Cube { }
    }
    \endcode

    For an illustration of its use see the flickr3d example.
*/
const char * Skybox::EXPECTED_NAMES[] = {
    "_west", "_up", "_east", "_down", "_south", "_north", 0 };

Skybox::Skybox(QObject *parent)
    : m_ready(false)
    , m_scene(0)
    , m_view(0)
    , m_camera(new QGLCamera(this))
    , m_bConnectedToOpenGLContextSignal(false)
{
    m_view = qobject_cast<Viewport*>(parent);

    m_camera->setEye(QVector3D());
    m_camera->setNearPlane(0.6f);
    m_camera->setViewSize(QSizeF(1.0f, 1.0f));

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
}

/*!
    \internal
    Destroy this skybox returning all resources
*/
Skybox::~Skybox()
{

}
/*!
    \internal

    Implement the event property to catch when the skybox is placed into a
    viewport parent.
*/
bool Skybox::event(QEvent *event)
{
    if (event->type() == QEvent::ParentChange && !m_view)
    {
        setViewport(qobject_cast<Viewport*>(parent()));
    }
    return QObject::event(event);
}

/*!
    \qmlproperty Viewport Mesh::viewport

    This property holds the viewport which this Skybox will be displayed
    inside.  The skybox will completely fill the viewport, but since it is
    drawn first, and with depth buffering off, all other content in the
    given viewport will overlay the skybox making it appear infinitely
    distant.

    Generally this property should not need to be set as the skybox can be
    simply placed inside the viewport element in which it should display.
*/
void Skybox::setViewport(Viewport *view)
{
    if (m_view != view)
    {
        m_view = view;
        emit viewportChanged();
    }
}

void Skybox::scanLocalDir(const QString &imagePath)
{
    m_imagePath = imagePath;
    m_bConnectedToOpenGLContextSignal = false;
    QStringList notFound;
    const char **exp = EXPECTED_NAMES;
    for ( ; *exp; ++exp)
        notFound << QLatin1String(*exp);
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
            for (int ix = 0; EXPECTED_NAMES[ix]; ++ix)
            {
                if (tok.contains(EXPECTED_NAMES[ix]))
                {
                    QUrl url;
                    url.setScheme("file");
                    url.setPath(ent.absoluteFilePath());
                    m_faces[ix]->material()->setTextureUrl(url);
                    m_faces[ix]->material()->texture()->setHorizontalWrap(QGL::ClampToEdge);
                    m_faces[ix]->material()->texture()->setVerticalWrap(QGL::ClampToEdge);
                    notFound.removeOne(QLatin1String(EXPECTED_NAMES[ix]));
                    break;
                }
            }
        }
        if (notFound.size() > 0)
        {
            qWarning() << imagePath << "did not contain a skybox image for" << notFound;
        }
    }
    else
    {
        qWarning() << imagePath << "was not a directory of skybox images";
    }
}

/*!
    \qmlproperty url Skybox::source

    The location where the 6 textures for the skybox are stored.  The textures are
    in any image format that Qt can load, but must have names that contain the following
    strings:
    \list
        \li "_up"
        \li "_down"
        \li "_west"
        \li "_east"
        \li "_north"
        \li "_south"
    \endlist
    These textures are then attached to the appropriate faces of the skybox.
*/
void Skybox::setSource(const QUrl &source)
{
    if (source != m_source)
    {
        m_source = source;
        if (m_source.scheme() == QLatin1String("file"))
        {
            scanLocalDir(m_source.toLocalFile());
        }
        else
        {
            // TODO - handle network case
            qWarning() << "Network support for skybox images not yet implemented";
        }
        emit sourceChanged();
    }
}

void Skybox::draw(QGLPainter *painter)
{
    if (!m_view)
        return;

    if (!m_bConnectedToOpenGLContextSignal) {
        QOpenGLContext* pOpenGLContext = QOpenGLContext::currentContext();
        if (pOpenGLContext) {
            bool Ok = QObject::connect(pOpenGLContext, SIGNAL(aboutToBeDestroyed()), this, SLOT(handleOpenglContextIsAboutToBeDestroyed()), Qt::DirectConnection);
            Q_UNUSED(Ok);  // quell compiler warning
            Q_ASSERT(Ok);
            m_bConnectedToOpenGLContextSignal = true;
        }
    }

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

void Skybox::handleOpenglContextIsAboutToBeDestroyed()
{
    for (int ix = 0; ix<6; ++ix) {
        m_faces[ix]->material()->texture()->cleanupResources();
    }
}

/*!
    \internal
*/
void Skybox::classBegin()
{
}

/*!
    \internal
    Checks that all loading and initialisation has been finished, and emits the  loaded() signal if
    the component is complete.

    \sa loaded()
*/
void Skybox::componentComplete()
{
}
