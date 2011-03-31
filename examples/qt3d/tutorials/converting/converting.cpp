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

//! [includes]
#include <QApplication>
#include <QtOpenGL/qgl.h>
#include "qglpainter.h"
//! [includes]

//! [class-defn]
class TutorialWidget : public QGLWidget
{
    Q_OBJECT
public:
    TutorialWidget(QWidget *parent=0) : QGLWidget(parent) {}

protected:
    void paintGL();
};
//! [class-defn]

//! [paintGL]
//! [clear-window]
void TutorialWidget::paintGL()
{
    QGLPainter painter(this);

    painter.setClearColor(palette().color(backgroundRole()));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//! [clear-window]

//! [set-matrices]
    QMatrix4x4 projm;
    projm.ortho(rect());
    painter.projectionMatrix() = projm;

    painter.modelViewMatrix().setToIdentity();
//! [set-matrices]

//! [draw-all]
//! [set-effect]
    painter.setStandardEffect(QGL::FlatColor);
    painter.setColor(QColor(170, 202, 0, 255));
//! [set-effect]

//! [create-vertex-array]
    QVector3DArray vertices;
    vertices.append(60.0f,  10.0f,  0.0f);
    vertices.append(110.0f, 110.0f, 0.0f);
    vertices.append(10.0f,  110.0f, 0.0f);
//! [create-vertex-array]

//! [draw]
    painter.clearAttributes();
    painter.setVertexAttribute(QGL::Position, vertices);
    painter.draw(QGL::Triangles, 3);
//! [draw]
//! [draw-all]
}
//! [paintGL]

//! [main]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TutorialWidget widget;
    widget.show();
    return app.exec();
}
//! [main]

#include "converting.moc"
