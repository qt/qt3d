/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "graph.h"
#include "edge.h"
#include "node.h"
#include "scene.h"

#include <QDebug>
#include <QGraphicsScene>

#include <math.h>

Graph::Graph(QObject *parent)
    : QObject(parent), timerId(0)
{
    QGraphicsEmbedScene *scene = new Scene(this);
    sceneObject = scene;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-128, -128, 256, 256);

    Node *node1 = new Node(this);
    Node *node2 = new Node(this);
    Node *node3 = new Node(this);
    Node *node4 = new Node(this);
    centerNode = new Node(this);
    Node *node6 = new Node(this);
    Node *node7 = new Node(this);
    Node *node8 = new Node(this);
    Node *node9 = new Node(this);
    scene->addItem(node1);
    scene->addItem(node2);
    scene->addItem(node3);
    scene->addItem(node4);
    scene->addItem(centerNode);
    scene->addItem(node6);
    scene->addItem(node7);
    scene->addItem(node8);
    scene->addItem(node9);
    scene->addItem(new Edge(node1, node2));
    scene->addItem(new Edge(node2, node3));
    scene->addItem(new Edge(node2, centerNode));
    scene->addItem(new Edge(node3, node6));
    scene->addItem(new Edge(node4, node1));
    scene->addItem(new Edge(node4, centerNode));
    scene->addItem(new Edge(centerNode, node6));
    scene->addItem(new Edge(centerNode, node8));
    scene->addItem(new Edge(node6, node9));
    scene->addItem(new Edge(node7, node4));
    scene->addItem(new Edge(node8, node7));
    scene->addItem(new Edge(node9, node8));

    node1->setPos(-50, -50);
    node2->setPos(0, -50);
    node3->setPos(50, -50);
    node4->setPos(-50, 0);
    centerNode->setPos(0, 0);
    node6->setPos(50, 0);
    node7->setPos(-50, 50);
    node8->setPos(0, 50);
    node9->setPos(50, 50);
}

void Graph::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void Graph::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}
