/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cubeview.h"
#include "qglcube.h"
#include "qglteapot.h"

#include <QtCore/qpropertyanimation.h>

#include <QtGui/QOpenGLFramebufferObject>

//! [1]
CubeView::CubeView(QWindow *parent)
    : QGLView(parent)
    , fbo(0)
    , tangle(0.0f)
    , cangle(0.0f)
    , oangle(0.0f)
{
    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    builder << QGLCube(1.5f);
    cube = builder.currentNode();
    cube->setObjectName(QLatin1String("Cube"));

    builder.newSection();
    builder << QGLTeapot();
    teapot = builder.currentNode();
    teapot->setObjectName(QLatin1String("Teapot"));

    scene = builder.finalizedSceneNode();
    scene->setParent(this);
//! [1]

//! [2]
    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(this, "teapotAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(1000);
    animation->setLoopCount(-1);
    animation->start();

    animation = new QPropertyAnimation(this, "cubeAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(5000);
    animation->setLoopCount(-1);
    animation->start();

    animation = new QPropertyAnimation(this, "orbitAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(5000);
    animation->setLoopCount(-1);
    animation->start();
//! [2]

//! [3]
    innerCamera = new QGLCamera(this);
}
//! [3]

CubeView::~CubeView()
{
    qtlogo.cleanupResources();
    delete fbo;
}

//! [4]
void CubeView::initializeGL(QGLPainter *)
{
    fbo = new QOpenGLFramebufferObject(512, 512, QOpenGLFramebufferObject::Depth);
    fboSurface.setFramebufferObject(fbo);
//! [4]

//! [5]
    QImage textureImage(QLatin1String(":/qtlogo.png"));
    qtlogo.setImage(textureImage);

    glEnable(GL_BLEND);
}
//! [5]

//! [6]
void CubeView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().push();
    painter->projectionMatrix().push();
    painter->pushSurface(&fboSurface);
//! [6]

//! [7]
    painter->setCamera(innerCamera);
    painter->modelViewMatrix().rotate(tangle, 0.0f, 1.0f, 0.0f);

    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));
    painter->setStandardEffect(QGL::LitMaterial);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    teapot->draw(painter);
//! [7]

//! [8]
    painter->popSurface();
    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();
//! [8]

//! [9]
    painter->modelViewMatrix().rotate(oangle, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 m = painter->modelViewMatrix();
    QVector3D cube1pos(-1.5f, 0.0f, 0.0f);
    QVector3D cube2pos(1.5f, 0.0f, 0.0f);

    if (m.map(cube1pos).z() < m.map(cube2pos).z()) {
        drawCube1(painter, cube1pos);
        drawCube2(painter, cube2pos);
    } else {
        drawCube2(painter, cube2pos);
        drawCube1(painter, cube1pos);
    }
}
//! [9]

//! [10]
void CubeView::drawCube1(QGLPainter *painter, const QVector3D &posn)
{
    painter->modelViewMatrix().push();

    qtlogo.bind();
    painter->setFaceColor(QGL::AllFaces, QColor(202, 100, 0, 150));
    painter->setStandardEffect(QGL::LitDecalTexture2D);

    painter->modelViewMatrix().translate(posn);
    painter->modelViewMatrix().rotate(cangle, 1.0f, -1.0f, 1.0f);

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    cube->draw(painter);
    glCullFace(GL_BACK);
    cube->draw(painter);
    glDisable(GL_CULL_FACE);

    painter->modelViewMatrix().pop();
}
//! [10]

//! [11]
void CubeView::drawCube2(QGLPainter *painter, const QVector3D &posn)
{
    painter->modelViewMatrix().push();

    painter->setFaceColor(QGL::AllFaces, QColor(0, 160, 202, 125));
    painter->setStandardEffect(QGL::LitDecalTexture2D);
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glEnable(GL_TEXTURE_2D);

    painter->modelViewMatrix().translate(posn);
    painter->modelViewMatrix().rotate(cangle, 1.0f, 1.0f, 1.0f);

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    cube->draw(painter);
    glCullFace(GL_BACK);
    cube->draw(painter);
    glDisable(GL_CULL_FACE);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    painter->modelViewMatrix().pop();
}
//! [11]
