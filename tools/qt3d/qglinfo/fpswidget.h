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

#ifndef FPSWIDGET_H
#define FPSWIDGET_H

#include <QGLWidget>
#if defined(QT_OPENGL_ES_2)
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/QGLShaderProgram>
#endif

QT_BEGIN_NAMESPACE
class QTime;
class QTimer;
QT_END_NAMESPACE

class FPSWidget : public QGLWidget
{
    Q_OBJECT
public:
    FPSWidget(QWidget *parent = 0);
    ~FPSWidget();
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
protected:
    void keyPressEvent(QKeyEvent *) { close(); }
    void mousePressEvent(QMouseEvent *) { close(); }
signals:
    void fps(int);
private slots:
    void animate();
private:
    QTime *time;
    QTime *frameTime;
    QTimer *timer;
    QTimer *goAwayTimer;
    int xrot, yrot, zrot;
    int frameElapsed;
    int frameCount;
    int totalFrameTime;
#if defined(QT_OPENGL_ES_2)
    void setupShaders();

    QGLShaderProgram program;
    QMatrix4x4 projection;
    QMatrix4x4 modelView;
    int vertexAttr;
    int normalAttr;
    int matrixUniform;
    int materialUniform;
#endif
};

#endif // FPSWIDGET_H
