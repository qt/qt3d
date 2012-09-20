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

#ifndef LOAD_MODEL_H
#define LOAD_MODEL_H

#include "qglview.h"

QT_BEGIN_NAMESPACE
//class QGLSceneNode;
class QGLAbstractScene;
QT_END_NAMESPACE

class LoadModelView : public QGLView
{
    Q_OBJECT
    Q_PROPERTY(float angleTeapot READ angleTeapot WRITE setAngleTeapot)
    Q_PROPERTY(float angleTeapotOrbit READ angleTeapotOrbit WRITE setAngleTeapotOrbit)
    Q_PROPERTY(float angleLander READ angleLander WRITE setAngleLander)
public:
    LoadModelView(QWindow *parent = 0);
    ~LoadModelView();

    float angleTeapot() const { return m_AngleTeapot; }
    void setAngleTeapot(float angle) { m_AngleTeapot = angle; update(); }

    float angleTeapotOrbit() const { return m_AngleTeapotOrbit; }
    void setAngleTeapotOrbit(float angle) { m_AngleTeapotOrbit = angle; update(); }

    float angleLander() const { return m_AngleLander; }
    void setAngleLander(float angle) { m_AngleLander = angle; update(); }

protected:
    void paintGL(QGLPainter *painter);

private:
    void loadModels();

    QGLAbstractScene*   m_pTeapotScene;
    QGLTexture2D*       m_pTeapotTexture;
    float               m_AngleTeapot;
    float               m_AngleTeapotOrbit;

    QGLAbstractScene*   m_pLanderScene;
    QGLTexture2D*       m_pLanderTexture;
    float               m_AngleLander;
};

#endif
