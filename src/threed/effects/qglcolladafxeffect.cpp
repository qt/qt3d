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

#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QImage>
#include "qgl.h"
#include "qgltexture2d.h"
#include "qglcolladafxeffect.h"
#include "qglcolladafxeffect_p.h"

QT_BEGIN_NAMESPACE

QGLColladaFxEffect::QGLColladaFxEffect() : QGLShaderProgramEffect()
        , d( new QGLColladaFxEffectPrivate )
{
}

QGLColladaFxEffect::QGLColladaFxEffect(const QGLColladaFxEffect&) : QGLShaderProgramEffect()
{
    Q_ASSERT(false);
};


QGLColladaFxEffect::~QGLColladaFxEffect()
{
    delete d;
}



QGLColladaFxEffectPrivate::QGLColladaFxEffectPrivate() : id()
        , sid()
        , name()
        , emissiveTexture(0)
        , ambientTexture(0)
        , diffuseTexture(0)
        , specularTexture(0)
        , lighting(QGLColladaFxEffect::NoLighting)
        , material(0)
{
    resetGlueSnippets();
}



QGLColladaFxEffectPrivate::~QGLColladaFxEffectPrivate()
{
    delete emissiveTexture;
    emissiveTexture = 0;
    delete ambientTexture;
    ambientTexture = 0;
    delete diffuseTexture;
    diffuseTexture = 0;
    delete specularTexture;
    specularTexture = 0;
    delete material;
    material = 0;

}



inline void QGLColladaFxEffectPrivate::updateMaterialChannelSnippets(QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor)
{
    QString qVariableName = QLatin1String("q") + channelName;
    if (texture != 0)
    {
        QString sourceVariableName = QLatin1String("texture") + channelName;
        QString texVariableName = QString(QLatin1String("texCoord%1")).arg(*textureUnit);
        // Take care of texture coordinates
        QString varyingSnippet = QString(QLatin1String("varying vec4 %1;")).arg(texVariableName);
        vertexShaderDeclarationSnippets.append(varyingSnippet);
        vertexShaderVariableNames.append(texVariableName);
        fragmentShaderDeclarationSnippets.append(varyingSnippet);
        fragmentShaderVariableNames.append(texVariableName);

        vertexShaderCodeSnippets.append(QString(QLatin1String("%1 = texCoords; // TODO: dynamically add tex attributes\n")).arg(texVariableName));
        vertexShaderVariableNames.append(texVariableName);

        // Declare the color variable in the fragment shader
        fragmentShaderDeclarationSnippets.append(QString(QLatin1String("lowp vec4 %1;")).arg(qVariableName));
        fragmentShaderVariableNames.append(qVariableName);
        fragmentShaderDeclarationSnippets.append(QString(QLatin1String("uniform sampler2D %1;")).arg(sourceVariableName));
        fragmentShaderVariableNames.append(sourceVariableName);

        // Assign a colour to the variable out of the appropriate sampler
        fragmentShaderCodeSnippets.append(QLatin1String("    mediump vec4 ") + qVariableName + QLatin1String(" = texture2D(") + sourceVariableName + QLatin1String(", ") + texVariableName + QLatin1String(".st);"));
        fragmentShaderVariableNames.append(qVariableName);
        // mediump? lowp?

        *textureUnit++;
    } else {
        fragmentShaderDeclarationSnippets.append(QString (QLatin1String("const vec4 %1 = vec4(%2, %3, %4, %5);")).arg( qVariableName).arg(fallbackColor.redF(), 0, 'f', 6).arg(fallbackColor.greenF(), 0, 'f', 6).arg(fallbackColor.blueF(), 0, 'f', 6).arg(fallbackColor.alphaF(), 0, 'f', 6 ));
        fragmentShaderVariableNames.append(qVariableName);
    }
}



inline void QGLColladaFxEffectPrivate::setTextureUniform(QGLShaderProgram *program, QGLPainter* painter, QString channelName, QGLTexture2D* texture, int* textureUnit, QColor fallbackColor)
{
    QString qVariableName = QLatin1String("q") + channelName;

    if (texture != 0)
    {
        QString sourceVariableName = QLatin1String("texture") + channelName;
        QString texVariableName = QString(QLatin1String("texCoord%1")).arg(*textureUnit);
        painter->glActiveTexture(GL_TEXTURE0 + *textureUnit);
        texture->bind();
        program->setUniformValue(sourceVariableName.toAscii().data(), *textureUnit);
    }
    else
    {
        // It's just a const value, so set it that way in the fragment shader.
        program->setUniformValue(qVariableName.toAscii().data(), fallbackColor);
    }
}



void QGLColladaFxEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    QGLShaderProgramEffect::update(painter, updates);

    if (updates && QGLPainter::UpdateMaterials)
    {

        if (program() == 0)
        {
            qWarning() << "no program in QGLColladaFxEffect::update()";
            return;
        }

        // Start from texture unit 1 so as not to stomp a texture set on the
        // painter.
        int textureUnit = 1;
        d->setTextureUniform(
                program(), painter, QLatin1String("Emissive"), d->emissiveTexture,
                &textureUnit,
                material() ? material()->emittedLight() : QColor());

        d->setTextureUniform(
                program(), painter, QLatin1String("Ambient"), d->ambientTexture, &textureUnit,
                material() ? material()->ambientColor() : QColor());

        d->setTextureUniform(
                program(), painter, QLatin1String("Diffuse"), d->diffuseTexture, &textureUnit,
                material() ? material()->diffuseColor() : QColor());

        d->setTextureUniform(
                program(), painter, QLatin1String("Specular"), d->specularTexture,
                &textureUnit,
                material() ? material()->specularColor() : QColor());
    }
}



void QGLColladaFxEffect::setId(QString id)
{
    d->id = id;
}



QString QGLColladaFxEffect::id()
{
    return d->id;
}



void QGLColladaFxEffect::setSid(QString sid)
{
    d->sid = sid;
}



void QGLColladaFxEffectPrivate::addMaterialChannelsToShaderSnippets(const QGLMaterial *material)
{
    int textureUnit = 1;

    updateMaterialChannelSnippets(QLatin1String("Emissive"), emissiveTexture, &textureUnit, material->emittedLight());
    updateMaterialChannelSnippets(QLatin1String("Ambient"), ambientTexture, &textureUnit, material->ambientColor());
    updateMaterialChannelSnippets(QLatin1String("Diffuse"), diffuseTexture, &textureUnit, material->diffuseColor());
    updateMaterialChannelSnippets(QLatin1String("Specular"), specularTexture, &textureUnit, material->specularColor());
}



void QGLColladaFxEffect::addBlinnPhongLighting()
{
    d->addMaterialChannelsToShaderSnippets(material());

    // Fragment shader declarations:
    d->fragmentShaderDeclarationSnippets.append(QLatin1String("uniform mediump sampler2D texture0;"));
    d->fragmentShaderVariableNames.append(QLatin1String("texture0"));
    d->fragmentShaderDeclarationSnippets.append(QLatin1String("varying highp vec4 qt_TexCoord0;"));
    d->fragmentShaderVariableNames.append(QLatin1String("qt_TexCoord0"));

    // Fragment Shader code
    d->fragmentShaderCodeSnippets.append(QLatin1String(
            "    vec4 specularComponent = vec4( 0.0, 0.0, 0.0, 0.0 );\n"\
            "    if (intensity > 0.0)\n"\
            "   {\n"\
            "       float specularIntensity = max( dot(perPixelNormal, qHalfVector), 0.0 );\n"\
            "       if (specularIntensity > 0.0)\n"\
            "           specularComponent = qSpecular  * pow(specularIntensity, shininess);\n"\
            "   }\n"));
    d->fragmentShaderVariableNames.append(QLatin1String("lighting"));


    // Replace the "end glue" to set colour from lighting
    d->fragmentShaderEndGlueSnippet = QLatin1String(
            "    vec4 texture0Color = texture2D(texture0, qt_TexCoord0.st);\n"\
            "    vec4 diffuseColor = qDiffuse;\n"\
            "    vec4 lightingColor = qAmbient + diffuseColor * intensity + specularComponent;\n"\
            "   vec4 texturedColor = vec4(lightingColor.xyz * (1.0 - texture0Color.a)\n"\
            "+ (texture0Color.xyz + specularComponent.rgb) * texture0Color.a, lightingColor.a);\n"\
            "    gl_FragColor = texturedColor;\n"\
            "}");
    generateShaders();
}



void QGLColladaFxEffect::generateShaders()
{
    if (vertexShader().isEmpty())
    {
        QString shader =
                d->vertexShaderDeclarationSnippets.join(QLatin1String("\n"))
                + QLatin1String("\n") + d->vertexShaderMainGlueSnippet
                + d->vertexShaderCodeSnippets.join(QLatin1String("\n"))
                + QLatin1String("\n") + d->vertexShaderEndGlueSnippet;
        setVertexShader(shader.toLatin1());
    }

    if (fragmentShader().isEmpty())
    {
        QString shader =
                d->fragmentShaderDeclarationSnippets.join(QLatin1String("\n"))
                + QLatin1String("\n") + d->fragmentShaderMainGlueSnippet
                +  d->fragmentShaderCodeSnippets.join(QLatin1String("\n"))
                + QLatin1String("\n") + d->fragmentShaderEndGlueSnippet;
        setFragmentShader(shader.toLatin1());
    }
}



void QGLColladaFxEffectPrivate::resetGlueSnippets()
{
    vertexShaderMainGlueSnippet = QLatin1String(
            "attribute highp vec4 vertex;\n"\
            "attribute highp vec4 normal;\n"\
            "attribute highp vec4 texCoords;\n"\
            "uniform highp mat4 matrix;\n"\
            "uniform highp mat3 qt_NormalMatrix;\n"\
            "varying mediump vec3 qNormal;\n"\
            "varying mediump vec3 qLightDirection;\n"\
            "varying mediump vec3 qHalfVector;\n"\
            "uniform mediump vec3 pli;       // Position of the light\n"\
            "varying highp vec4 qt_TexCoord0; // TEMP\n" /* Got to get rid of this*/\
            "\n"\
            "void qLightVertex(vec4 vertex, vec3 normal)\n"\
            "{\n"\
            "    vec3 toEye;\n"\
            "    qLightDirection = normalize(pli);\n"\
            "    toEye = vec3(0, 0, 1); // assume viewer at infinity\n"\
            "    qHalfVector = normalize(qLightDirection + toEye);\n"\
            "}\n"\
            "\n"\
            "void main(void)\n"\
            "{\n"\
            "    qNormal = normalize(qt_NormalMatrix * vec3(normal));\n"\
            "    qLightVertex(vertex, qNormal);\n"\
            "    qt_TexCoord0 = texCoords;\n"\
            );

    vertexShaderEndGlueSnippet = QLatin1String (
            "    gl_Position = matrix * vertex;\n"\
            "}\n");


    fragmentShaderEndGlueSnippet = QLatin1String(
            "    gl_FragColor = color;\n"\
            "}\n"
            );

    fragmentShaderMainGlueSnippet = QLatin1String(
            "varying mediump vec3 qNormal;\n"\
            "varying mediump vec3 qLightDirection;\n"\
            "varying mediump vec3 qHalfVector;\n"\
            "uniform float shininess;\n"\
            "uniform vec4 color;\n"\
            "vec3 perPixelNormal;"
            "\n"\
            "void main()\n"\
            "{\n"\
            "    perPixelNormal = normalize(qNormal);\n"\
            "    float intensity =  max(dot(perPixelNormal, qLightDirection), 0.0);\n"
            );
}



QString QGLColladaFxEffect::sid()
{
    return d->sid;
}



QGLTexture2D* QGLColladaFxEffect::diffuseTexture()
{
    return d->diffuseTexture;
}


void QGLColladaFxEffect::setLighting(int lighting)
{
    d->lighting = lighting;
}

int QGLColladaFxEffect::lighting()
{
    return d->lighting;
}

/*!
  Sets this effect to use \a newMaterial.  If \a newMaterial is 0, sets this
  effect to have no material, and instead use whatever material is set
  on the QGLPainter.

  \sa QGLPainter, material()
*/
void QGLColladaFxEffect::setMaterial(QGLMaterial* newMaterial)
{
    d->material = newMaterial;
}

/*!
    Returns a pointer to the material of this effect.  If the effect has no material,
    this function returns 0;
*/
QGLMaterial* QGLColladaFxEffect::material()
{
    return d->material;
}

/*!
  Returns true if the effect is currently active (applied to a QGLPainter)
  and false if it is not.
  */
bool QGLColladaFxEffect::isActive()
{
    return d->currentlyActive;
}

void QGLColladaFxEffect::setActive(QGLPainter *painter, bool flag)
{
    d->currentlyActive = flag && !vertexShader().isEmpty() &&
                         !fragmentShader().isEmpty();
    QGLShaderProgramEffect::setActive(painter, d->currentlyActive);
}

QT_END_NAMESPACE
