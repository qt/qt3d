/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

attribute highp vec4 vertex;
attribute highp vec4 texCoords;

uniform highp mat4 matrix;
uniform highp mat4 qt_ProjectionMatrix;
uniform highp mat4 qt_ModelViewMatrix;

uniform highp mat4 cameraCombinedMatrix;
uniform highp mat4 lightCombinedMatrix;

uniform highp mat4 cameraModelViewMatrix;
uniform highp mat4 inverseCameraModelViewMatrix;
uniform highp mat4 lightModelViewMatrix;

uniform highp mat4 cameraProjectionMatrix;
uniform highp mat4 lightProjectionMatrix;

varying highp vec4 qt_TexCoord0;
varying highp vec4 onScreenPosition;
varying highp vec4 lightScreenPosition;
varying highp vec4 worldSpaceVertex;
varying float lightDepth;
varying float actualDepth;

void main(void)
{
    qt_TexCoord0 = texCoords;
//    onScreenPosition = matrix * vertex;
//onScreenPosition = cameraCombinedMatrix * vertex; // works
    onScreenPosition = qt_ProjectionMatrix *
    qt_ModelViewMatrix
    * vertex;

    highp vec4 worldPosition = inverseCameraModelViewMatrix *
        qt_ModelViewMatrix * vertex;

//    lightScreenPosition = lightCombinedMatrix * vertex;
//    lightScreenPosition = lightCombinedMatrix * worldPosition;
    lightScreenPosition = lightProjectionMatrix * lightModelViewMatrix * vertex;
    lightDepth = lightScreenPosition.z;

    actualDepth = onScreenPosition.z;

    worldSpaceVertex = vertex;

//    gl_Position = qt_ProjectionMatrix * worldPosition;
//gl_Position = onScreenPosition;
gl_Position = onScreenPosition;
//    gl_Position = lightScreenPosition;
};
