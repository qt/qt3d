/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D examples of the Qt Toolkit.
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "view.h"
#include "cubeitem.h"
#include "teapotitem.h"
#include "graph.h"
#include "qglcamera.h"
#include <QtGui/qgraphicssceneevent.h>

View::View(QWidget *parent)
    : QGraphicsView(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    scene.setBackgroundBrush(Qt::black);
    setScene(&scene);

    cube = new CubeItem();
    cube->setRect(0, 0, 600, 480);
    cube->camera()->setEye(QVector3D(-5.0f, 3.0f, 6.0f));
    cube->setVisible(false);

    teapot = new TeapotItem();
    teapot->setRect(0, 0, 600, 480);
    teapot->camera()->setEye(QVector3D(-5.0f, 3.0f, 6.0f));

    Button *cubeButton = new Button
        (QPixmap(QLatin1String(":/cubeicon.png")), teapot);
    Button *teapotButton = new Button
        (QPixmap(QLatin1String(":/teapoticon.png")), cube);

    connect(cubeButton, SIGNAL(clicked()), this, SLOT(switchToCube()));
    connect(teapotButton, SIGNAL(clicked()), this, SLOT(switchToTeapot()));

    scene.addItem(cube);
    scene.addItem(teapot);

    Graph *graph = new Graph(this);
    cube->setScene(graph->scene());
    teapot->setScene(graph->scene());
}

void View::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    cube->setRect(0, 0, width(), height());
    teapot->setRect(0, 0, width(), height());
}

void View::switchToCube()
{
    teapot->setVisible(false);
    cube->setVisible(true);
}

void View::switchToTeapot()
{
    cube->setVisible(false);
    teapot->setVisible(true);
}

Button::Button(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
    , pressed(false)
{
    setFlag(ItemIsSelectable, true);
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed = true;
    QGraphicsItem::mousePressEvent(event);
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (pressed && boundingRect().contains(event->scenePos()))
        emit clicked();
    pressed = false;
    QGraphicsItem::mouseReleaseEvent(event);
}
