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

#ifndef BUILDER_H
#define BUILDER_H

#include "qglview.h"
#include "qgraphicsrotation3d.h"

QT_BEGIN_NAMESPACE
class QGLSceneNode;
class QGLBuilder;
class QGLShaderProgramEffect;
QT_END_NAMESPACE

class SolarSystemView : public QGLView
{
    Q_OBJECT
    Q_PROPERTY(float angle1 READ angle1 WRITE setAngle1)
    Q_PROPERTY(float angle2 READ angle2 WRITE setAngle2)
    Q_PROPERTY(float angle3 READ angle3 WRITE setAngle3)
    Q_PROPERTY(float glowFactor READ glowFactor WRITE setGlowFactor)
public:
    SolarSystemView(QWindow *parent = 0);
    ~SolarSystemView();

    float angle1() const { return m_angle1; }
    void setAngle1(float angle) { m_angle1 = angle; update(); }

    float angle2() const { return m_angle2; }
    void setAngle2(float angle) { m_angle2 = angle; update(); }

    float angle3() const { return m_angle3; }
    void setAngle3(float angle) { m_angle3 = angle; update(); }

    float glowFactor() const { return m_glowFactor; }
    void setGlowFactor(float arg) { m_glowFactor = arg; update(); }

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

private:
    float m_angle1;
    float m_angle2;
    float m_angle3;
    float m_glowFactor;
    QGraphicsRotation3D *sunRotation;
    QGraphicsRotation3D *planetRotation;
    QGraphicsRotation3D *moonRotation;
    QGraphicsRotation3D *systemRotation;
    QGLSceneNode *spaceScene;
    QList<QGLTexture2D*> m_LoadedTextures;

    QGLShaderProgramEffect* sunEffect;

    QGLSceneNode *createScene();
};

#endif
