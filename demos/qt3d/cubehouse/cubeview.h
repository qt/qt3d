/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
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
    Q_PROPERTY(qreal cubeAngle READ cubeAngle WRITE setCubeAngle)
public:
    CubeView(QWidget *parent = 0);
    ~CubeView() {}

    void setShowFrameRate(bool value) { showFrameRate = value; }
    void setStereo(bool value) { stereo = value; }
    void setProjectiveTextureEffect(bool value)
{
    useProjectiveTextureEffect = value;
}

    qreal cubeAngle() const { return cangle; }
    void setCubeAngle(qreal angle);

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
    qreal sensitivity;
    QGLLightModel *roomModel;
    QGLLightModel *normalModel;
    QGLLightParameters *lightParameters;
    bool showFrameRate;
    bool stereo;
    bool useProjectiveTextureEffect;
    QTime time;
    qreal cangle;
    mutable qreal prevX, prevY, prevZ;
    mutable bool havePrev;

    QVector3D gravity() const;

    ProjectiveTextureEffect* projectiveTextureEffect;
    QMatrix4x4 biasMatrix;
    QMatrix4x4Stack modelMatrix;
    QMatrix4x4 objectLinearTexgenMatrix;
};

#endif
