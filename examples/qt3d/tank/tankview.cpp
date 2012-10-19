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

#include "tankview.h"
#include "tank.h"
#include "quadplane.h"

#include "qglscenenode.h"
#include "qray3d.h"
#include "qplane3d.h"

#include <QMouseEvent>

TankView::TankView(QWindow *parent)
    : QGLView(parent)
    , m_tankScene(new QGLSceneNode)
    , m_count(0)
{
    m_tankScene->setParent(this);
    m_tankScene->setPalette(QSharedPointer<QGLMaterialCollection>(new QGLMaterialCollection(this)));
    m_tankScene->setEffect(QGL::LitDecalTexture2D);
    m_tankScene->addNode(new QuadPlane(0, QSizeF(25, 25)));
    Tank *tank = addTank();
    connect(tank, SIGNAL(updated()), this, SLOT(update()));

    // TODO: setToolTip not implemented in QWindow
    // setToolTip(tr("Double-click to add more tanks"));
    setTitle(tr("Double-click Me!"));
}

TankView::~TankView()
{
}

Tank *TankView::addTank()
{
    Tank *tank = new Tank(m_tankScene);
    tank->setObjectName(QString(QLatin1String("Tank %1")).arg(m_count));
    ++m_count;
    connect(tank, SIGNAL(updated()), this, SLOT(update()));
    return tank;
}

void TankView::paintGL(QGLPainter *painter)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_tankScene->draw(painter);
}

void TankView::initializeGL(QGLPainter *painter)
{
    painter->setClearColor(Qt::gray);
    camera()->setEye(camera()->eye() + QVector3D(0.0f, 4.0f, 0.0f));
    glEnable(GL_BLEND);
}

void TankView::mouseDoubleClickEvent(QMouseEvent *e)
{
    Tank *t = addTank();
    QVector3D pick = mapPoint(e->pos());

    // find the origin of the near plane
    QRay3D eyeline(camera()->eye(), (camera()->center() - camera()->eye()).normalized());
    QVector3D nearPlaneOrigin = eyeline.point(camera()->nearPlane());

    // from the near plane origin move up and across by the pick's XY to find the point
    // on the near plane
    QRay3D up(nearPlaneOrigin, camera()->upVector());
    QVector3D sideDir = QVector3D::crossProduct(camera()->upVector(), -eyeline.direction());
    QRay3D side(up.point(pick.y()), sideDir.normalized());
    QVector3D v = side.point(pick.x());

    // intersect the ray thru the picked point on the near plane with the floor
    QRay3D ray(camera()->eye(), v - camera()->eye());
    QPlane3D floorPlane(QVector3D(), QVector3D(0, 1, 0));
    QVector3D pointOnFloor = ray.point(floorPlane.intersection(ray));
    t->setPosition(pointOnFloor);
}
