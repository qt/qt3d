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

#include <QFile>
#include "projectivetextureeffect.h"

#include <QOpenGLShaderProgram>

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
    if (!program())
        return;

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
