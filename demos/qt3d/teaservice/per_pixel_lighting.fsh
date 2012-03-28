/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

// Per-pixel lighting - fragment shader side.

struct qt_MaterialParameters {
    mediump vec4 emission;
    mediump vec4 ambient;
    mediump vec4 diffuse;
    mediump vec4 specular;
    mediump float shininess;
};
uniform qt_MaterialParameters qt_Material;

struct qt_SingleLightParameters {
    mediump vec4 position;
    mediump vec3 spotDirection;
    mediump float spotExponent;
    mediump float spotCutoff;
    mediump float spotCosCutoff;
    mediump float constantAttenuation;
    mediump float linearAttenuation;
    mediump float quadraticAttenuation;
};
uniform qt_SingleLightParameters qt_Light;

varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;
varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;

vec4 qLightPixel(vec4 ambient, vec4 diffuse)
{
    float angle, spot;
    vec4 color;
    vec4 component;
    vec3 normal = normalize(qNormal);

    // Start with the ambient color.
    color = ambient;

    // Determine the cosine of the angle between the normal and the
    // vector from the vertex to the light.
    angle = max(dot(normal, qLightDirection), 0.0);

    // Calculate the diffuse light components.
    component = angle * diffuse;

    // Calculate the specular light components.
    if (angle != 0.0) {
        angle = max(dot(normal, qHalfVector), 0.0);
        component += pow(angle, qt_Material.shininess) * qt_Material.specular;
    }

    // Apply the spotlight angle and exponent.
    if (qt_Light.spotCutoff != 180.0) {
        spot = max(dot(normalize(qVertexToLight),
                       normalize(qt_Light.spotDirection)), 0.0);
        if (spot < qt_Light.spotCosCutoff)
            spot = 0.0;
        else
            spot = pow(spot, qt_Light.spotExponent);
        component *= spot;
    }

    return clamp(color + component, 0.0, 1.0);
}

void main(void)
{
    gl_FragColor = qLightPixel(qAmbient, qDiffuse);
}
