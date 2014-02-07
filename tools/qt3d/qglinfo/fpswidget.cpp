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

#include "fpswidget.h"

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>

static GLfloat materialColor[] = { 0.1f, 0.85f, 0.25f, 1.0f };

// dont drive the updates any faster than every this many millisecondss
#define MAX_ANIM_FREQUENCY 5

#define QGL_CUBE_SIZE (6 * 6 * (3 + 3))
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
};

FPSWidget::FPSWidget(QWidget *parent)
    : QGLWidget(parent)
    , time(new QTime())
    , frameTime(new QTime())
    , timer(new QTimer(this))
    , goAwayTimer(new QTimer(this))
    , xrot(0)
    , yrot(0)
    , zrot(0)
    , frameElapsed(0)
    , frameCount(0)
    , totalFrameTime(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Generating Frames/Second"));
    time->start();
    frameTime->start();
    connect(timer, SIGNAL(timeout()),
            this, SLOT(animate()));
    timer->start(0);   // call the animation as soon as we return to the event loop
    goAwayTimer = new QTimer(this);
    connect(goAwayTimer, SIGNAL(timeout()),
            this, SLOT(close()));
    goAwayTimer->start(30000);
}

FPSWidget::~FPSWidget()
{
    delete time;
}

#if defined(QT_OPENGL_ES_2)

void FPSWidget::setupShaders()
{
    QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
    const char *vsrc = {
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump float angle;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.5, 5.0, 7.0));\n"
        "    angle = max(dot(normal, toLight), 0.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n"
    };
    vshader->compileSourceCode(vsrc);

    QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);
    const char *fsrc = {
        "uniform mediump vec4 material;\n"
        "varying mediump float angle;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = material * 0.2 + material * 0.8 * angle;\n"
        "}\n"
    };
    fshader->compileSourceCode(fsrc);

    program.addShader(vshader);
    program.addShader(fshader);
    program.link();

    vertexAttr = program.attributeLocation("vertex");
    normalAttr = program.attributeLocation("normal");
    matrixUniform = program.uniformLocation("matrix");
    materialUniform = program.uniformLocation("material");

    program.bind();
}

#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

void FPSWidget::initializeGL()
{
    glClearColor(0.9f, 0.95f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
#if defined(QT_OPENGL_ES_2)
    setupShaders();
#else
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
#endif
}

void FPSWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
#if !defined(QT_OPENGL_ES_2)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#endif
    int side = qMin(w, h);
    float ws = float(w) / float(side);
    float hs = float(h) / float(side);
#if defined(QT_OPENGL_ES_2)
    projection.setToIdentity();
    projection.ortho(-ws, ws, -hs, hs, -10.0f, 10.0f);
#else
    glOrtho(-ws, ws, -hs, hs, -10.0f, 10.0f);
#endif
}

void FPSWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if !defined(QT_OPENGL_ES_2)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
    glRotatef(zrot, 0.0, 0.0, 1.0);

    glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), cubeVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 6 * sizeof(GLfloat), cubeVertices);
    glEnableClientState(GL_NORMAL_ARRAY);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
#else
    modelView.setToIdentity();
    modelView.rotate(xrot, 1.0f, 0.0f, 0.0f);
    modelView.rotate(yrot, 0.0f, 1.0f, 0.0f);
    modelView.rotate(zrot, 0.0f, 0.0f, 1.0f);
    program.setUniformValue(matrixUniform, projection * modelView);

    QVector4D mat(materialColor[0], materialColor[1],
                  materialColor[2], materialColor[3]);
    program.setUniformValue(materialUniform, mat);

    program.setAttributeArray(vertexAttr, cubeVertices, 3, 6 * sizeof(GLfloat));
    program.setAttributeArray(normalAttr, cubeVertices+3, 3, 6 * sizeof(GLfloat));
    program.enableAttributeArray(vertexAttr);
    program.enableAttributeArray(normalAttr);
#endif

    glDrawArrays(GL_TRIANGLES, 0, 36);

#if !defined(QT_OPENGL_ES_2)
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
#else
    program.disableAttributeArray(vertexAttr);
    program.disableAttributeArray(normalAttr);
#endif
    QString framesPerSecond = QString::fromLatin1("%1 fps").arg(1000.0f / (float(totalFrameTime) / float(frameCount)));
}

void FPSWidget::animate()
{
    int elapsed = time->restart();
    frameElapsed += elapsed;
    if (frameElapsed > MAX_ANIM_FREQUENCY)
    {
        frameElapsed += elapsed;
        int adv = frameElapsed / 10;
        if (adv > 0)
        {
            frameCount += 1;
            totalFrameTime += frameElapsed;
            emit fps(1000.0f / (float(frameTime->elapsed()) / float(frameCount)));
            xrot = (xrot + adv) % 360;
            yrot = (xrot + adv) % 360;
            zrot = (xrot + adv) % 360;
            update();
            frameElapsed = 0;
        }
    }
}
