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

#include <QFile>
#include "projectivetextureeffect.h"
#include <QtOpenGL/qglshaderprogram.h>

static const QMatrix4x4 biasMatrix = QMatrix4x4(0.5, 0.0, 0.0, 0.5,
                                         0.0, 0.5, 0.0, 0.5,
                                         0.0, 0.0, 0.5, 0.5,
                                         0.0, 0.0, 0.0, 1.0);

/*!
  The ProjectiveTextureEffect mimics the effect of shining a projector onto
  a scene from a specific direction.  Vertex coordinates in object space are
  transformed into eye-space coordinates relative to the light direction,
  using the objectLinearTexgenMatrix.
*/
ProjectiveTextureEffect::ProjectiveTextureEffect() :
    matrixDirty(true)
{
    setupShaders();
}

void ProjectiveTextureEffect::setActive(QGLPainter *painter, bool flag)
{
    QGLShaderProgramEffect::setActive(painter, flag);
}

void ProjectiveTextureEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    QGLShaderProgramEffect::update(painter, updates);

    if (matrixDirty)
    {
        recalulateObjectLinearTexgenMatrix();
        matrixDirty = false;
    }

    program()->setUniformValue("objectLinearTexgenMatrix",
                               objectLinearTexgenMatrix);
    program()->setUniformValue("projectorDirection",
                               projectorDirection);
}

void ProjectiveTextureEffect::setProjectorDirection(const QVector4D &direction)
{
    this->projectorDirection = direction;
    matrixDirty = true;
}

void ProjectiveTextureEffect::setCameraModelViewMatrix(const QMatrix4x4 &newCameraModelViewMatrix)
{
    cameraModelViewMatrix = newCameraModelViewMatrix;
    bool invertible;
    inverseCameraModelViewMatrix =
            newCameraModelViewMatrix.inverted(&invertible);
    Q_ASSERT(invertible);
    if (!invertible)
        qWarning() << "camera Model view matrix not invertible in ProjectiveDepthTestEffect::setCameraModelViewMatrix()";
    matrixDirty = true;
}

void ProjectiveTextureEffect::setProjectorProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    projectorProjectionMatrix = newMatrix;
    matrixDirty = true;
}

void ProjectiveTextureEffect::setProjectorViewMatrix(const QMatrix4x4 &newMatrix)
{
    projectorViewMatrix = newMatrix;
    matrixDirty = true;
}

void ProjectiveTextureEffect::setModelMatrix(const QMatrix4x4 &newMatrix)
{
    modelMatrix = newMatrix;
    matrixDirty = true;
}

void ProjectiveTextureEffect::recalulateObjectLinearTexgenMatrix()
{
    objectLinearTexgenMatrix = biasMatrix *
                projectorProjectionMatrix *
                projectorViewMatrix *
                modelMatrix;
}

void ProjectiveTextureEffect::setupShaders()
{
    QString vertexShaderFileName = QLatin1String(":/shaders/objectlineartexgen.vert");
    QFile vertexShaderFile(vertexShaderFileName);
    if (vertexShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setVertexShader(vertexShaderFile.readAll());
    } else {
        qWarning() << "Could not open file "<<vertexShaderFileName<<", failed to load vertex shader";
    }

    QString fragmentShaderFileName = QLatin1String(":/shaders/objectlineartexgen.frag");
    QFile fragmentShaderFile(fragmentShaderFileName);
    if (fragmentShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setFragmentShader(fragmentShaderFile.readAll());
    } else {
        qWarning() << "Could not open file "<<fragmentShaderFileName<<", failed to load fragment shader";
    }

}
