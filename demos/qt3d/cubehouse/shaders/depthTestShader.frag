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

// This is equivilent to GL_REPLACE
varying highp vec4 qt_TexCoord0;

uniform mediump sampler2D texture;
uniform mediump mat4 textureMatrix;
varying highp vec4 onScreenPosition;
varying highp vec4 lightScreenPosition;
varying highp vec4 worldSpaceVertex;
varying float lightDepth;
varying float actualDepth;

void main(void)
{

    // I should be able to build a matrix to do this:
//    vec4 normalizedOnScreenPosition = (onScreenPosition + 5.0) / 10.0;
vec4 normalizedLightScreenPosition = (((lightScreenPosition
//+ lightScreenPosition.w
) / lightScreenPosition.w ) + 1.0) / 2.0;

//    float lightMapDepth = texture2D(texture, normalizedLightScreenPosition.st).z;
    float lightMapDepth = texture2D(texture, normalizedLightScreenPosition.st ).z;
//        gl_FragColor = vec4(lightMapDepth, lightMapDepth, lightMapDepth, 1.0);

//    gl_FragColor = vec4(normalizedLightScreenPosition.st,0.0,1.0);

//    float lightActualDepth = normalizedLightScreenPosition.z;
    float lightActualDepth = lightDepth;
//        gl_FragColor = vec4(lightActualDepth, lightActualDepth, lightActualDepth, 1.0);
    float lightActualDepthWithStupidMaths = (lightActualDepth + 10.0) / 20.0;
    float lightDepthWithStupidMaths = (lightDepth + 10.0) / 20.0;
//    gl_FragColor = vec4(vec3(lightActualDepthWithStupidMaths), 1.0);


//    if (lightActualDepthWithStupidMaths > lightMapDepth )
//    gl_FragColor = vec4(0.2, 0.2, 0.2, 1.0);
//    else
    gl_FragColor = vec4(1.0, 1.0, 0.5, 1.0);


//    gl_FragColor = vec4(lightDepthWithStupidMaths, lightDepthWithStupidMaths, lightDepthWithStupidMaths, 1.0);


//float stupidMathslightDepth = (lightDepth + 10.0) / 20;
//gl_FragColor = vec4(stupidMathslightDepth, stupidMathslightDepth, stupidMathslightDepth, 1.0);

//float stupidMathsActualDepth = (actualDepth + 10.0) / 20.0;
//gl_FragColor = vec4(stupidMathsActualDepth, stupidMathsActualDepth, stupidMathsActualDepth, 1.0);

};
