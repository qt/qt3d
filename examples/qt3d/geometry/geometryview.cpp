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

#include "geometryview.h"
#include "qray3d.h"

#include <QImage>
#include <QPainter>
#include <QDir>
#include <QTimer>
#include <QWheelEvent>

#include <math.h>

#include "geometry.h"
#include "quadplane.h"
#include "qplane3d.h"
#include "qglmaterialcollection.h"
#include "qgraphicsscale3d.h"

GeometryView::GeometryView(QWindow *parent)
    : QGLView(parent)
    , timer(new QTimer(this))
{
    lp = new QGLLightParameters(this);
    lp->setPosition(QVector3D(-3.0, 1.0, 15.0));
    lp->setSpotAngle(15.0);
    lp->setSpotExponent(24);
    lp->setSpotDirection(QVector3D(0.0, -1.0, -2.0));

    QSharedPointer<QGLMaterialCollection> palette(new QGLMaterialCollection());

    // first set up the geometry - an icosahedron
    // this one handles all its own colors and textures
    icosahedron = new Geometry(this, palette);

    // now a generic flat floor plane which will be painted grey
    // and textured the same as the icosahedron
    floor = new QuadPlane(this);
    floor->setPalette(palette);
    QGLMaterial *parms = new QGLMaterial;
    parms->setAmbientColor(Qt::darkGray);
    parms->setDiffuseColor(Qt::gray);
    int m = palette->addMaterial(parms);
    QGLTexture2D *tex = icosahedron->material()->texture();
    parms->setTexture(tex);
    floor->setMaterialIndex(m);
    floor->setPosition(QVector3D(0, 0, -5));
    QGraphicsScale3D *scale = new QGraphicsScale3D(floor);
    scale->setScale(QVector3D(0.2f, 0.2f, 0.1f));
    floor->addTransform(scale);

    mdl = new QGLLightModel(this);
    mdl->setAmbientSceneColor(QColor(196,196,196));
}

GeometryView::~GeometryView()
{
    delete icosahedron;
    delete floor;
    delete timer;
}

void GeometryView::initializeGL(QGLPainter *painter)
{
    painter->setLightModel(mdl);

    painter->setMainLight(lp);

    connect(timer, SIGNAL(timeout()), this, SLOT(rotate()));
    timer->start(25);
}

void GeometryView::paintGL(QGLPainter *painter)
{
    floor->draw(painter);
    painter->modelViewMatrix().rotate(angle, 0.10f, 1.0f, 0.0f);
    icosahedron->draw(painter);
}

void GeometryView::rotate()
{
    angle = (angle + 2) % 360;
    update();
}
