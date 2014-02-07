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

#ifndef CUBEVIEW_H
#define CUBEVIEW_H

#include "qglview.h"
#include "qglbuilder.h"
#include "qgltexture2d.h"
#include <QtCore/qdatetime.h>

class ProjectiveTextureEffect;

class CubeView : public QGLView
{
    Q_OBJECT
    Q_PROPERTY(float cubeAngle READ cubeAngle WRITE setCubeAngle)
public:
    CubeView(QWindow *parent = 0);
    ~CubeView();

    void setShowFrameRate(bool value) { showFrameRate = value; }
    void setStereo(bool value) { stereo = value; }
    void setProjectiveTextureEffect(bool value)
{
    useProjectiveTextureEffect = value;
}

    float cubeAngle() const { return cangle; }
    void setCubeAngle(float angle);

protected:
    void initializeGL(QGLPainter *painter);
    void paintGL(QGLPainter *painter);

private slots:
    void accelerometerTimeout();
    void updateProjectorViewMatrix();
    void updateProjectorProjectionMatrix();
    void updateProjectiveTextureEffect();

private:
    QGLTexture2D texture;
    QGLSceneNode *scene;
    QGLSceneNode *cube;
    QGLSceneNode *teapot;
    QGLSceneNode *room;
    QGLCamera *roomCamera;
    QGLCamera *projectorCamera;
    float sensitivity;
    QGLLightModel *roomModel;
    QGLLightModel *normalModel;
    QGLLightParameters *lightParameters;
    bool showFrameRate;
    bool stereo;
    bool useProjectiveTextureEffect;
    QTime time;
    float cangle;
    mutable float prevX, prevY, prevZ;
    mutable bool havePrev;

    QVector3D gravity() const;

    ProjectiveTextureEffect* projectiveTextureEffect;
    QMatrix4x4 biasMatrix;
    QMatrix4x4Stack modelMatrix;
    QMatrix4x4 objectLinearTexgenMatrix;
};

#endif
