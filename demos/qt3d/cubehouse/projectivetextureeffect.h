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

#ifndef PROJECTIVEDTEXTUREEFFECT_H
#define PROJECTIVEDTEXTUREEFFECT_H

#include "qglshaderprogrameffect.h"
//#include "deptheffect.h"
#include "qmatrix4x4.h"

class ProjectiveTextureEffect : public QGLShaderProgramEffect
{
public:
    explicit ProjectiveTextureEffect();
    virtual void setActive(QGLPainter *painter, bool flag);
    virtual void update(QGLPainter *painter, QGLPainter::Updates updates);
    virtual void setProjectorDirection(const QVector4D &direction);

    void setCameraModelViewMatrix(const QMatrix4x4 &newCameraModelViewMatrix);
    void setProjectorProjectionMatrix(const QMatrix4x4 &newMatrix);
    void setProjectorViewMatrix(const QMatrix4x4 &newMatrix);
    void setModelMatrix(const QMatrix4x4 &newMatrix);

//      TODO:
//    QMatrix4x4 eyeLinearTexgenMatrix;

protected:
    virtual void setupShaders();
private:
    virtual void recalulateObjectLinearTexgenMatrix();
    bool matrixDirty;
    QMatrix4x4 modelMatrix;
    QMatrix4x4 objectLinearTexgenMatrix;
    QMatrix4x4 cameraModelViewMatrix;
    QMatrix4x4 inverseCameraModelViewMatrix;
    QMatrix4x4 projectorProjectionMatrix;
    QMatrix4x4 projectorViewMatrix;
    QVector4D projectorDirection;
};

#endif // PROJECTIVEDTEXTUREEFFECT_H
