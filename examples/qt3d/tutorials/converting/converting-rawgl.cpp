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

//! [class-defn]
#include <QApplication>
#include <QtOpenGL/qgl.h>

class TutorialWidget : public QGLWidget
{
    Q_OBJECT
public:
    TutorialWidget(QWidget *parent=0) : QGLWidget(parent) {}

protected:
    void resizeGL(int width, int height);
    void paintGL();
#if defined(QT_OPENGL_ES_2)
    void initializeGL();

private:
    GLuint program;
    GLuint vertexAttr;
    GLuint matrixUniform;
    GLuint colorUniform;
#endif
};
//! [class-defn]

//! [set-viewport]
void TutorialWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
//! [set-viewport]

#if !defined(QT_OPENGL_ES_2)

//! [clear-window]
void TutorialWidget::paintGL()
{
    QColor clearColor(palette().color(backgroundRole()));
    glClearColor(clearColor.redF(), clearColor.greenF(),
                 clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//! [clear-window]

//! [set-matrices]
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width(), height(), 0, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//! [set-matrices]

//! [draw]
    QColor color(170, 202, 0, 255);
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());

    static GLfloat vertices[] = {
        60.0f,  10.0f,  0.0f,
        110.0f, 110.0f, 0.0f,
        10.0f,  110.0f, 0.0f
    };

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
//! [draw]
}

#else // QT_OPENGL_ES_2

void TutorialWidget::initializeGL()
{
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    const char *vsrc[1] = {
        "attribute highp vec4 vertex;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n"
    };
    glShaderSource(vshader, 1, vsrc, 0);
    glCompileShader(vshader);

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsrc[1] = {
        "uniform mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n"
    };
    glShaderSource(fshader, 1, fsrc, 0);
    glCompileShader(fshader);

    program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    vertexAttr = glGetAttribLocation(program, "vertex");
    matrixUniform = glGetUniformLocation(program, "matrix");
    colorUniform = glGetUniformLocation(program, "color");

    glUseProgram(program);
}

void TutorialWidget::paintGL()
{
    QColor clearColor(palette().color(backgroundRole()));
    glClearColor(clearColor.redF(), clearColor.greenF(),
                 clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat width = this->width();
    GLfloat height = this->height();
    GLfloat left = 0.0f;
    GLfloat right = width;
    GLfloat top = 0.0f;
    GLfloat bottom = height;
    GLfloat nearPlane = -1.0f;
    GLfloat farPlane = 1.0f;
    GLfloat clip = farPlane - nearPlane;
    GLfloat mat[4][4];
    mat[0][0] = 2.0f / width;
    mat[1][0] = 0.0f;
    mat[2][0] = 0.0f;
    mat[3][0] = -(left + right) / width;
    mat[0][1] = 0.0f;
    mat[1][1] = 2.0f / -height;
    mat[2][1] = 0.0f;
    mat[3][1] = -(top + bottom) / -height;
    mat[0][2] = 0.0f;
    mat[1][2] = 0.0f;
    mat[2][2] = -2.0f / clip;
    mat[3][2] = -(nearPlane + farPlane) / clip;
    mat[0][3] = 0.0f;
    mat[1][3] = 0.0f;
    mat[2][3] = 0.0f;
    mat[3][3] = 1.0f;
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, mat[0]);

    QColor color(170, 202, 0, 255);
    glUniform4f(colorUniform, color.redF(), color.greenF(), color.blueF(), color.alphaF());

    static GLfloat vertices[] = {
        60.0f,  10.0f,  0.0f,
        110.0f, 110.0f, 0.0f,
        10.0f,  110.0f, 0.0f
    };

    glVertexAttribPointer(vertexAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(vertexAttr);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(vertexAttr);
}

#endif // QT_OPENGL_ES_2

//! [main]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TutorialWidget widget;
    widget.show();
    return app.exec();
}
//! [main]

#include "converting-rawgl.moc"
