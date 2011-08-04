/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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

#include "qgllitmaterialeffect_p.h"
#include "qglabstracteffect_p.h"
#include "qglext_p.h"
#include <QtOpenGL/qglshaderprogram.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLLitMaterialEffect
    \since 4.8
    \brief The QGLLitMaterialEffect class provides a standard effect that draws fragments with a lit material.
    \ingroup qt3d
    \ingroup qt3d::painting
    \internal
*/

#if !defined(QGL_FIXED_FUNCTION_ONLY)

static char const litMaterialVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec3 normal;\n"
    "uniform highp mat4 matrix;\n"
    "uniform highp mat4 modelView;\n"
    "uniform highp mat3 normalMatrix;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "    highp vec4 tvertex = modelView * vertex;\n"
    "    highp vec3 norm = normalize(normalMatrix * normal);\n"
    "    qLightVertex(tvertex, norm);\n"
    "}\n";

static char const litMaterialFragmentShader[] =
#if !defined(QT_OPENGL_ES)
    "varying mediump vec4 qColor;\n"
    "varying mediump vec4 qSecondaryColor;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
    "}\n";
#else
    "varying mediump vec4 qCombinedColor;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = qCombinedColor;\n"
    "}\n";
#endif

// Algorithm from section 2.14.1 of OpenGL 2.1 specification.
static char const litMaterialLightingShader[] =
#if !defined(QT_OPENGL_ES)
"uniform mediump vec3 sdli;\n"   // Direction of the light (must be normalized).
"uniform mediump vec3 pli;\n"    // Position of the light
"uniform mediump float pliw;\n"  // 0 for directional, 1 for positional.
"uniform mediump float srli;\n"  // Spotlight exponent for the light
"uniform mediump float crli;\n"  // Spotlight cutoff for the light
"uniform mediump float ccrli;\n" // Cosine of spotlight cutoff for the light
"uniform mediump float k0;\n"    // Constant attentuation factor for the light
"uniform mediump float k1;\n"    // Linear attentuation factor for the light
"uniform mediump float k2;\n"    // Quadratic attentuation factor for the light
"uniform mediump vec4 acm[2];\n" // Ambient color of the material and light
"uniform mediump vec4 dcm[2];\n" // Diffuse color of the material and light
"uniform mediump vec4 scm[2];\n" // Specular color of the material and light
"uniform mediump vec4 ecm[2];\n" // Emissive color and ambient scene color
"uniform mediump float srm[2];\n"// Specular exponent of the material
"uniform bool viewerAtInfinity;\n" // Light model indicates viewer at infinity
"uniform bool twoSided;\n"       // Light model indicates two-sided lighting

"varying mediump vec4 qColor;\n"
"varying mediump vec4 qSecondaryColor;\n"

"void qLightVertex(vec4 vertex, vec3 normal)\n"
"{\n"
"    int i, material;\n"
"    vec3 toEye, toLight, h;\n"
"    float angle, spot, attenuation;\n"
"    vec4 color, scolor;\n"
"    vec4 adcomponent, scomponent;\n"

    // Determine which material to use.
"    if (!twoSided || normal.z >= 0.0) {\n"
"        material = 0;\n"
"    } else {\n"
"        material = 1;\n"
"        normal = -normal;\n"
"    }\n"

    // Start with the material's emissive color and the ambient scene color,
    // which have been combined into the ecm parameter by the C++ code.
"    color = ecm[material];\n"
"    scolor = vec4(0, 0, 0, 0);\n"

    // Vector from the vertex to the eye position (i.e. the origin).
"    if (viewerAtInfinity)\n"
"        toEye = vec3(0, 0, 1);\n"
"    else\n"
"        toEye = normalize(-vertex.xyz);\n"

    // Determine the cosine of the angle between the normal and the
    // vector from the vertex to the light.
"    if (pliw == 0.0)\n"
"        toLight = normalize(pli);\n"
"    else\n"
"        toLight = normalize(pli - vertex.xyz);\n"
"    angle = max(dot(normal, toLight), 0.0);\n"

    // Calculate the ambient and diffuse light components.
"    adcomponent = acm[material] + angle * dcm[material];\n"

    // Calculate the specular light components.
"    if (angle != 0.0) {\n"
"        h = normalize(toLight + toEye);\n"
"        angle = max(dot(normal, h), 0.0);\n"
"        if (srm[material] != 0.0)\n"
"            scomponent = pow(angle, srm[material]) * scm[material];\n"
"        else\n"
"            scomponent = scm[material];\n"
"    } else {\n"
"        scomponent = vec4(0, 0, 0, 0);\n"
"    }\n"

    // Apply the spotlight angle and exponent.
"    if (crli != 180.0) {\n"
"        spot = max(dot(normalize(vertex.xyz - pli), sdli), 0.0);\n"
"        if (spot < ccrli) {\n"
"            adcomponent = vec4(0, 0, 0, 0);\n"
"            scomponent = vec4(0, 0, 0, 0);\n"
"        } else {\n"
"            spot = pow(spot, srli);\n"
"            adcomponent *= spot;\n"
"            scomponent *= spot;\n"
"        }\n"
"    }\n"

    // Apply attenuation to the colors.
"    if (pliw != 0.0) {\n"
"        attenuation = k0;\n"
"        if (k1 != 0.0 || k2 != 0.0) {\n"
"            float len = length(pli - vertex.xyz);\n"
"            attenuation += k1 * len + k2 * len * len;\n"
"        }\n"
"        color += adcomponent / attenuation;\n"
"        scolor += scomponent / attenuation;\n"
"    } else {\n"
"        color += adcomponent;\n"
"        scolor += scomponent;\n"
"    }\n"

    // Generate the final output colors.
"    float alpha = dcm[material].a;\n"
"    qColor = vec4(clamp(color.rgb, 0.0, 1.0), alpha);\n"
"    qSecondaryColor = clamp(scolor, 0.0, 1.0);\n"
"}\n";
#else
"uniform mediump vec3 sdli;\n"   // Direction of the light (must be normalized).
"uniform mediump vec3 pli;\n"    // Position of the light
"uniform mediump float pliw;\n"  // 0 for directional, 1 for positional.
"uniform mediump float srli;\n"  // Spotlight exponent for the light
"uniform mediump float crli;\n"  // Spotlight cutoff for the light
"uniform mediump float ccrli;\n" // Cosine of spotlight cutoff for the light
"uniform mediump vec4 acm;\n"    // Ambient color of the material and light
"uniform mediump vec4 dcm;\n"    // Diffuse color of the material and light
"uniform mediump vec4 scm;\n"    // Specular color of the material and light
"uniform mediump vec4 ecm;\n"    // Emissive color and ambient scene color
"uniform mediump float srm;\n"   // Specular exponent of the material
"uniform bool viewerAtInfinity;\n" // Light model indicates viewer at infinity

"varying mediump vec4 qColor;\n"
"varying mediump vec4 qSecondaryColor;\n"
"varying mediump vec4 qCombinedColor;\n"

"void qLightVertex(vec4 vertex, vec3 normal)\n"
"{\n"
"    vec3 toEye, toLight, h;\n"
"    float angle, spot;\n"
"    vec4 color, scolor;\n"

    // Vector from the vertex to the eye position (i.e. the origin).
"    if (viewerAtInfinity)\n"
"        toEye = vec3(0, 0, 1);\n"
"    else\n"
"        toEye = normalize(-vertex.xyz);\n"

    // Determine the cosine of the angle between the normal and the
    // vector from the vertex to the light.
"    if (pliw == 0.0)\n"
"        toLight = normalize(pli);\n"
"    else\n"
"        toLight = normalize(pli - vertex.xyz);\n"
"    angle = max(dot(normal, toLight), 0.0);\n"

    // Calculate the ambient and diffuse light components.
"    color = acm + angle * dcm;\n"

    // Calculate the specular light components.
"    if (angle != 0.0) {\n"
"        h = normalize(toLight + toEye);\n"
"        angle = max(dot(normal, h), 0.0);\n"
"        if (srm != 0.0)\n"
"            scolor = pow(angle, srm) * scm;\n"
"        else\n"
"            scolor = scm;\n"
"    } else {\n"
"        scolor = vec4(0, 0, 0, 0);\n"
"    }\n"

    // Apply the spotlight angle and exponent.
"    if (crli != 180.0) {\n"
"        spot = max(dot(normalize(vertex.xyz - pli), sdli), 0.0);\n"
"        if (spot < ccrli) {\n"
"            color = vec4(0, 0, 0, 0);\n"
"            scolor = vec4(0, 0, 0, 0);\n"
"        } else {\n"
"            spot = pow(spot, srli);\n"
"            color *= spot;\n"
"            scolor *= spot;\n"
"        }\n"
"    }\n"

    // Generate the final output colors.
"    color += ecm;\n"
"    float alpha = dcm.a;\n"
    // Note: Normally, the qCombinedColor is ignored, and per-pixel
    // value is calculated.
    // If OPENGL_ES is defined, qColor and qSecondaryColor are ignored,
    // and qCombinedColor is calculated here to speed up the fragment shader.
"    qColor = vec4(clamp(color.rgb, 0.0, 1.0), alpha);\n"
"    qSecondaryColor = clamp(scolor, 0.0, 1.0);\n"
"    qCombinedColor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
"}\n";
#endif

static QByteArray createVertexSource(const char *lighting, const char *extra)
{
    QByteArray contents(lighting);
    return contents + extra;
}

static inline QVector4D colorToVector4(const QColor& color)
{
    return QVector4D(color.redF(), color.greenF(),
                     color.blueF(), color.alphaF());
}

// Combine a material and light color into a single color.
static inline QVector4D colorToVector4
    (const QColor &color, const QColor &lightColor)
{
    return QVector4D(color.redF() * lightColor.redF(),
                     color.greenF() * lightColor.greenF(),
                     color.blueF() * lightColor.blueF(),
                     color.alphaF() * lightColor.alphaF());
}

#endif

class QGLLitMaterialEffectPrivate
{
public:
    QGLLitMaterialEffectPrivate()
        : program(0)
        , matrixUniform(-1)
        , modelViewUniform(-1)
        , normalMatrixUniform(-1)
        , textureMode(0)
#if !defined(QGL_FIXED_FUNCTION_ONLY)
        , vertexShader(litMaterialVertexShader)
        , fragmentShader(litMaterialFragmentShader)
#else
        , vertexShader(0)
        , fragmentShader(0)
#endif
        , programName(QLatin1String("qt.color.material"))
        , isFixedFunction(false)
    {
    }

    QGLShaderProgram *program;
    int matrixUniform;
    int modelViewUniform;
    int normalMatrixUniform;
    GLenum textureMode;
    const char *vertexShader;
    const char *fragmentShader;
    QString programName;
    bool isFixedFunction;
};

/*!
    Constructs a new lit material effect.
*/
QGLLitMaterialEffect::QGLLitMaterialEffect()
    : d_ptr(new QGLLitMaterialEffectPrivate)
{
}

/*!
    \internal
*/
QGLLitMaterialEffect::QGLLitMaterialEffect
        (GLenum mode, const char *vshader, const char *fshader,
         const QString& programName)
    : d_ptr(new QGLLitMaterialEffectPrivate)
{
    Q_D(QGLLitMaterialEffect);
    d->textureMode = mode;
    d->vertexShader = vshader;
    d->fragmentShader = fshader;
    d->programName = programName;
}

/*!
    Destroys this lit material effect.
*/
QGLLitMaterialEffect::~QGLLitMaterialEffect()
{
}

/*!
    \reimp
*/
void QGLLitMaterialEffect::setActive(QGLPainter *painter, bool flag)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    Q_UNUSED(painter);
    Q_D(QGLLitMaterialEffect);
    if (flag) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        if (d->textureMode) {
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, d->textureMode);
            glEnable(GL_TEXTURE_2D);
        }
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        if (d->textureMode) {
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_2D);
        }
    }
#else
    Q_UNUSED(painter);
    Q_D(QGLLitMaterialEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (painter->isFixedFunction()) {
        d->isFixedFunction = true;
        if (flag) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            if (d->textureMode) {
                qt_gl_ClientActiveTexture(GL_TEXTURE0);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, d->textureMode);
                glEnable(GL_TEXTURE_2D);
            }
        } else {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            if (d->textureMode) {
                qt_gl_ClientActiveTexture(GL_TEXTURE0);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                glDisable(GL_TEXTURE_2D);
            }
        }
        return;
    }
#endif
    QGLShaderProgram *program = painter->cachedProgram(d->programName);
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, createVertexSource(litMaterialLightingShader, d->vertexShader));
        program->addShaderFromSourceCode(QGLShader::Fragment, d->fragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("normal", QGL::Normal);
        if (d->textureMode != 0)
            program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
        if (!program->link()) {
            qWarning("QGLLitMaterialEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        painter->setCachedProgram(d->programName, program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        d->modelViewUniform = program->uniformLocation("modelView");
        d->normalMatrixUniform = program->uniformLocation("normalMatrix");
        program->bind();
        if (d->textureMode != 0) {
            program->setUniformValue("tex", 0);
            program->enableAttributeArray(QGL::TextureCoord0);
        }
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Normal);
    } else if (flag) {
        d->matrixUniform = program->uniformLocation("matrix");
        d->modelViewUniform = program->uniformLocation("modelView");
        d->normalMatrixUniform = program->uniformLocation("normalMatrix");
        program->bind();
        if (d->textureMode != 0) {
            program->setUniformValue("tex", 0);
            program->enableAttributeArray(QGL::TextureCoord0);
        }
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Normal);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::Normal);
        if (d->textureMode != 0)
            program->disableAttributeArray(QGL::TextureCoord0);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLLitMaterialEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    painter->updateFixedFunction
        (updates & (QGLPainter::UpdateMatrices |
                    QGLPainter::UpdateLights |
                    QGLPainter::UpdateMaterials));
#else
    Q_D(QGLLitMaterialEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->isFixedFunction) {
        painter->updateFixedFunction
            (updates & (QGLPainter::UpdateMatrices |
                        QGLPainter::UpdateLights |
                        QGLPainter::UpdateMaterials));
        return;
    }
#endif
    QGLShaderProgram *program = d->program;
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        program->setUniformValue(d->matrixUniform, painter->combinedMatrix());
        program->setUniformValue(d->modelViewUniform, painter->modelViewMatrix());
        program->setUniformValue(d->normalMatrixUniform, painter->normalMatrix());
    }
    const QGLLightParameters *lparams = painter->mainLight();
    QMatrix4x4 ltransform = painter->mainLightTransform();
    const QGLLightModel *model = painter->lightModel();
    if ((updates & (QGLPainter::UpdateLights | QGLPainter::UpdateMaterials)) != 0) {
        // Set the uniform variables for the light.
        program->setUniformValue
            ("sdli", lparams->eyeSpotDirection(ltransform).normalized());
        QVector4D pli = lparams->eyePosition(ltransform);
        program->setUniformValue("pli", QVector3D(pli.x(), pli.y(), pli.z()));
        program->setUniformValue("pliw", GLfloat(pli.w()));
        program->setUniformValue("srli", GLfloat(lparams->spotExponent()));
        program->setUniformValue("crli", GLfloat(lparams->spotAngle()));
        program->setUniformValue("ccrli", GLfloat(lparams->spotCosAngle()));
#if !defined(QT_OPENGL_ES)
        // Attenuation is not supported under ES, for performance.
        program->setUniformValue("k0", GLfloat(lparams->constantAttenuation()));
        program->setUniformValue("k1", GLfloat(lparams->linearAttenuation()));
        program->setUniformValue("k2", GLfloat(lparams->quadraticAttenuation()));
#endif

        // Set the uniform variables for the light model.
#if !defined(QT_OPENGL_ES)
        program->setUniformValue("twoSided", (int)(model->model() == QGLLightModel::TwoSided));
#endif
        program->setUniformValue("viewerAtInfinity", (int)(model->viewerPosition() == QGLLightModel::ViewerAtInfinity));
#if !defined(QT_OPENGL_ES)
        if (d->textureMode != 0)
            program->setUniformValue("separateSpecular", (int)(model->colorControl() == QGLLightModel::SeparateSpecularColor));
#endif

        // Set the uniform variables for the front and back materials.
#if defined(QT_OPENGL_ES)
        static const int MaxMaterials = 1;
#else
        static const int MaxMaterials = 2;
#endif
        QVector4D acm[MaxMaterials];
        QVector4D dcm[MaxMaterials];
        QVector4D scm[MaxMaterials];
        QVector4D ecm[MaxMaterials];
        float srm[MaxMaterials];
        const QGLMaterial *mparams = painter->faceMaterial(QGL::FrontFaces);
        acm[0] = colorToVector4(mparams->ambientColor(), lparams->ambientColor());
        dcm[0] = colorToVector4(mparams->diffuseColor(), lparams->diffuseColor());
        scm[0] = colorToVector4(mparams->specularColor(), lparams->specularColor());
        ecm[0] = colorToVector4(mparams->emittedLight()) +
                 colorToVector4(mparams->ambientColor(),
                                model->ambientSceneColor());
        srm[0] = (float)(mparams->shininess());
#if !defined(QT_OPENGL_ES)
        mparams = painter->faceMaterial(QGL::BackFaces);
        acm[1] = colorToVector4(mparams->ambientColor(), lparams->ambientColor());
        dcm[1] = colorToVector4(mparams->diffuseColor(), lparams->diffuseColor());
        scm[1] = colorToVector4(mparams->specularColor(), lparams->specularColor());
        ecm[1] = colorToVector4(mparams->emittedLight()) +
                 colorToVector4(mparams->ambientColor(),
                                model->ambientSceneColor());
        srm[1] = (float)(mparams->shininess());
#endif
        program->setUniformValueArray("acm", (const GLfloat *)acm, MaxMaterials, 4);
        program->setUniformValueArray("dcm", (const GLfloat *)dcm, MaxMaterials, 4);
        program->setUniformValueArray("scm", (const GLfloat *)scm, MaxMaterials, 4);
        program->setUniformValueArray("ecm", (const GLfloat *)ecm, MaxMaterials, 4);
        program->setUniformValueArray("srm", srm, MaxMaterials, 1);
    }
#endif
}

QT_END_NAMESPACE
