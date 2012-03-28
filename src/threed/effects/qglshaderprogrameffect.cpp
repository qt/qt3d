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

#include "qglshaderprogrameffect.h"
#include "qglabstracteffect_p.h"
#include <QtOpenGL/qglshaderprogram.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLShaderProgramEffect
    \since 4.8
    \brief The QGLShaderProgramEffect class provides applications with the ability to use shader programs written in GLSL as effects for 3D rendering.
    \ingroup qt3d
    \ingroup qt3d::painting

    \section1 Writing portable shaders

    Shader programs can be difficult to reuse across OpenGL implementations
    because of varying levels of support for standard vertex attributes and
    uniform variables.  In particular, GLSL/ES lacks all of the
    standard variables that are present on desktop OpenGL systems:
    \c{gl_Vertex}, \c{gl_Normal}, \c{gl_Color}, and so on.  Desktop OpenGL
    lacks the variable qualifiers \c{highp}, \c{mediump}, and \c{lowp}.

    QGLShaderProgramEffect is built on top of
    \l{http://doc.qt.nokia.com/4.7/qglshaderprogram.html}{QGLShaderProgram},
    which makes the process of writing portable shaders easier by
    prefixing all shader programs with the following lines on desktop OpenGL:

    \code
    #define highp
    #define mediump
    #define lowp
    \endcode

    This makes it possible to run most GLSL/ES shader programs
    on desktop systems.  The programmer should also restrict themselves
    to just features that are present in GLSL/ES, and avoid
    standard variable names that only work on the desktop.

    QGLShaderProgramEffect also defines some standard attribute and uniform
    variable names that all shaders are expected to use.  The following
    sections define these standard names.

    \section1 Attributes

    QGLShaderProgramEffect provides a standard set of 8 named vertex
    attributes that can be provided via QGLPainter::setVertexBundle():

    \table
    \header \o Shader Variable \o Mesh Attribute \o Purpose
    \row \o \c qt_Vertex \o QGL::Position
         \o The primary position of the vertex.
    \row \o \c qt_Normal \o QGL::Normal
         \o The normal at each vertex, for lit material effects.
    \row \o \c qt_Color \o QGL::Color
         \o The color at each vertex, for per-vertex color effects.
    \row \o \c qt_MultiTexCoord0 \o QGL::TextureCoord0
         \o The texture co-ordinate at each vertex for texture unit 0.
    \row \o \c qt_MultiTexCoord1 \o QGL::TextureCoord1
         \o Secondary texture co-ordinate at each vertex.
    \row \o \c qt_MultiTexCoord2 \o QGL::TextureCoord2
         \o Tertiary texture co-ordinate at each vertex.
    \row \o \c qt_Custom0 \o QGL::CustomVertex0
         \o First custom vertex attribute that can be used for any
            user-defined purpose.
    \row \o \c qt_Custom1 \o QGL::CustomVertex1
         \o Second custom vertex attribute that can be used for any
            user-defined purpose.
    \endtable

    These attributes may be used in the vertexShader(), as in the following
    example of a simple texture shader:

    \code
    attribute highp vec4 qt_Vertex;
    attribute highp vec4 qt_MultiTexCoord0;
    uniform mediump mat4 qt_ModelViewProjectionMatrix;
    varying highp vec4 qt_TexCoord0;

    void main(void)
    {
        gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
        qt_TexCoord0 = qt_MultiTexCoord0;
    }
    \endcode

    \section1 Uniform variables

    QGLShaderProgramEffect provides a standard set of uniform variables for
    common values from the QGLPainter environment:

    \table
    \header \o Shader Variable \o Purpose
    \row \o \c qt_ModelViewProjectionMatrix
         \o Combination of the modelview and projection matrices into a
            single 4x4 matrix.
    \row \o \c qt_ModelViewMatrix
         \o Modelview matrix without the projection.  This is typically
            used for performing calculations in eye co-ordinates.
    \row \o \c qt_ProjectionMatrix
         \o Projection matrix without the modelview.
    \row \o \c qt_NormalMatrix
         \o Normal matrix, which is the transpose of the inverse of the
            top-left 3x3 part of the modelview matrix.  This is typically
            used in lighting calcuations to transform \c qt_Normal.
    \row \o \c qt_WorldMatrix
         \o Modelview matrix without the eye position and orientation
            component.  See QGLPainter::worldMatrix() for further
            information.
    \row \o \c qt_Texture0
         \o Sampler corresponding to the texture on unit 0.
    \row \o \c qt_Texture1
         \o Sampler corresponding to the texture on unit 1.
    \row \o \c qt_Texture2
         \o Sampler corresponding to the texture on unit 2.
    \row \o \c qt_Color
         \o Set to the value of the QGLPainter::color() property.
            This is typically used for flat-color shaders that do
            not involve lighting.  Note that this is different from
            the \c qt_Color attribute, which provides per-vertex colors.
    \endtable

    The above variables are usually declared in the shaders as follows
    (where \c highp may be replaced with \c mediump or \c lowp depending
    upon the shader's precision requirements):

    \code
    uniform highp mat4 qt_ModelViewProjectionMatrix;
    uniform highp mat4 qt_ModelViewMatrix;
    uniform highp mat4 qt_ProjectionMatrix;
    uniform highp mat3 qt_NormalMatrix;
    uniform sampler2D qt_Texture0;
    uniform sampler2D qt_Texture1;
    uniform sampler2D qt_Texture2;
    uniform highp vec4 qt_Color;
    \endcode

    \section1 Material parameters

    QGLShaderProgramEffect will provide information about the front and
    back materials from QGLPainter::faceMaterial() if the
    \c qt_Materials array is present in the shader code.
    The array should be declared as follows:

    \code
    struct qt_MaterialParameters {
        mediump vec4 emission;
        mediump vec4 ambient;
        mediump vec4 diffuse;
        mediump vec4 specular;
        mediump float shininess;
    };
    uniform qt_MaterialParameters qt_Materials[2];
    \endcode

    The front material will be provided as index 0 and the back
    material will be provided as index 1.  If the shader only
    needs a single material, then the \c qt_Material variable
    can be declared instead:

    \code
    uniform qt_MaterialParameters qt_Material;
    \endcode

    The front material will be provided as the value of \c qt_Material
    and the back material will be ignored.

    Note: the \c emission parameter is actually QGLMaterial::emittedLight()
    combined with the QGLLightModel::ambientSceneColor() and
    QGLMaterial::ambientColor().  This helps simplify lighting shaders.

    \section1 Lighting parameters

    QGLShaderProgramEffect will provide information about the current lights
    specified on the QGLPainter if the \c qt_Lights array is present
    in the shader code.  The array should be declared as follows:

    \code
    struct qt_LightParameters {
        mediump vec4 ambient;
        mediump vec4 diffuse;
        mediump vec4 specular;
        mediump vec4 position;
        mediump vec3 spotDirection;
        mediump float spotExponent;
        mediump float spotCutoff;
        mediump float spotCosCutoff;
        mediump float constantAttenuation;
        mediump float linearAttenuation;
        mediump float quadraticAttenuation;
    };
    const int qt_MaxLights = 8;
    uniform qt_LightParameters qt_Lights[qt_MaxLights];
    uniform int qt_NumLights;
    \endcode

    As shown, up to 8 lights can be supported at once.  Usually this is
    more lights than most shaders need, and so the user can change the
    \c qt_MaxLights constant to a smaller value if they wish.  Be sure
    to also call setMaximumLights() to tell QGLShaderProgramEffect about
    the new light count limit.

    The \c qt_NumLights uniform variable will be set to the actual number
    of lights that are active on the QGLPainter when update() is called.

    Because most shaders will only need a single light, the shader can
    declare the \c qt_Light variable instead of the \c qt_Lights array:

    \code
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
    \endcode

    Note that we have omitted the \c ambient, \c diffuse, and \c specular
    colors for the single light.  QGLShaderProgramEffect will combine the material
    and light colors ahead of time into \c qt_Material or \c qt_Materials.
    This makes lighting shaders more efficient because they do not have
    to compute \c material_color * \c light_color; just \c material_color
    is sufficient.

    \section1 Varying variables

    The name and purpose of the varying variables is up to the
    author of the vertex and fragment shaders, but the following names
    are recommended for texture co-ordinates:

    \table
    \header \o Varying Variable \o Purpose
    \row \o \c qt_TexCoord0
         \o Texture coordinate for unit 0, copied from the \c qt_MultiTexCoord0
            attribute.
    \row \o \c qt_TexCoord1
         \o Texture coordinate for unit 1, copied from the \c qt_MultiTexCoord1
            attribute.
    \row \o \c qt_TexCoord2
         \o Texture coordinate for unit 2, copied from the \c qt_MultiTexCoord2
            attribute.
    \endtable

    \section1 Lighting shader example

    The following example demonstrates what a lighting shader that
    uses a single light, a single material, and a texture might look like.
    The shader is quite complex but demonstrates most of the features that
    can be found in the lighting implementation of a fixed-function
    OpenGL pipeline:

    \code
    attribute highp vec4 qt_Vertex;
    uniform highp mat4 qt_ModelViewProjectionMatrix;
    attribute highp vec3 qt_Normal;
    uniform highp mat4 qt_ModelViewMatrix;
    uniform highp mat3 qt_NormalMatrix;
    attribute highp vec4 qt_MultiTexCoord0;
    varying highp vec4 qt_TexCoord0;

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

    varying mediump vec4 litColor;
    varying mediump vec4 litSecondaryColor;

    void main(void)
    {
        gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
        gTexCoord0 = qt_MultiTexCoord0;

        // Calculate the vertex and normal to use for lighting calculations.
        highp vec4 vertex = qt_ModelViewMatrix * qt_Vertex;
        highp vec3 normal = normalize(qt_NormalMatrix * qt_Normal);

        // Start with the material's emissive color and the ambient scene color,
        // which have been combined into the emission parameter.
        vec4 color = ggl_Material.emission;

        // Viewer is at infinity.
        vec3 toEye = vec3(0, 0, 1);

        // Determine the angle between the normal and the light direction.
        vec4 pli = qt_Light.position;
        vec3 toLight;
        if (pli.w == 0.0)
            toLight = normalize(pli.xyz);
        else
            toLight = normalize(pli.xyz - vertex.xyz);
        float angle = max(dot(normal, toLight), 0.0);

        // Calculate the ambient and diffuse light components.
        vec4 adcomponent = qt_Material.ambient + angle * qt_Material.diffuse;

        // Calculate the specular light components.
        vec4 scomponent;
        if (angle != 0.0) {
            vec3 h = normalize(toLight + toEye);
            angle = max(dot(normal, h), 0.0);
            float srm = qt_Material.shininess;
            vec4 scm = qt_Material.specular;
            if (srm != 0.0)
                scomponent = pow(angle, srm) * scm;
            else
                scomponent = scm;
        } else {
            scomponent = vec4(0, 0, 0, 0);
        }

        // Apply the spotlight angle and exponent.
        if (qt_Light.spotCutoff != 180.0) {
            float spot = max(dot(normalize(vertex.xyz - pli.xyz),
                                 qt_Light.spotDirection), 0.0);
            if (spot < qt_Light.spotCosCutoff) {
                adcomponent = vec4(0, 0, 0, 0);
                scomponent = vec4(0, 0, 0, 0);
            } else {
                spot = pow(spot, qt_Light.spotExponent);
                adcomponent *= spot;
                scomponent *= spot;
            }
        }

        // Apply attenuation to the colors.
        if (pli.w != 0.0) {
            float attenuation = qt_Light.constantAttenuation;
            float k1 = qt_Light.linearAttenuation;
            float k2 = qt_Light.quadraticAttenuation;
            if (k1 != 0.0 || k2 != 0.0) {
                float len = length(pli.xyz - vertex.xyz);
                attenuation += k1 * len + k2 * len * len;
            }
            color += adcomponent / attenuation;
            scolor += scomponent / attenuation;
        } else {
            color += adcomponent;
            scolor += scomponent;
        }

        // Generate the final output colors to pass to the fragment shader.
        float alpha = qt_Material.diffuse.a;
        litColor = vec4(clamp(color.rgb, 0.0, 1.0), alpha);
        litSecondaryColor = vec4(clamp(scolor.rgb, 0.0, 1.0), 0.0);
    }
    \endcode

    The corresponding fragment shader is as follows:

    \code
    varying mediump vec4 litColor;
    varying mediump vec4 litSecondaryColor;
    varying highp vec4 qt_TexCoord0;

    void main(void)
    {
        vec4 color = litColor * texture2D(qt_Texture0, qt_TexCoord0.st);
        gl_FragColor = clamp(color + litSecondaryColor, 0.0, 1.0);
    }
    \endcode

    \section1 Fixed function operation

    If the OpenGL implementation does not support shaders, then
    QGLShaderProgramEffect will fall back to a flat color effect based
    on QGLPainter::color().  It is recommended that the application
    consult QGLPainter::isFixedFunction() to determine if some
    other effect should be used instead.
*/

class QGLShaderProgramEffectPrivate
{
public:
    QGLShaderProgramEffectPrivate()
        : geometryInputType(GL_TRIANGLE_STRIP)
        , geometryOutputType(GL_TRIANGLE_STRIP)
        , maximumLights(8)
        , attributes(0)
        , regenerate(true)
        , fixedFunction(false)
#if !defined(QGL_FIXED_FUNCTION_ONLY)
        , program(0)
        , matrix(-1)
        , mvMatrix(-1)
        , projMatrix(-1)
        , normalMatrix(-1)
        , worldMatrix(-1)
        , texture0(-1)
        , texture1(-1)
        , texture2(-1)
        , color(-1)
        , numLights(-1)
        , haveLight(0)
        , haveLights(0)
        , haveMaterial(0)
        , haveMaterials(0)
#endif
    {
    }
    ~QGLShaderProgramEffectPrivate()
    {
#if !defined(QGL_FIXED_FUNCTION_ONLY)
        delete program;
#endif
    }

    QByteArray vertexShader;
    QByteArray fragmentShader;
    QByteArray geometryShader;
    GLenum geometryInputType;
    GLenum geometryOutputType;
    int maximumLights;
    int attributes;
    bool regenerate;
    bool fixedFunction;
#if !defined(QGL_FIXED_FUNCTION_ONLY)
    QGLShaderProgram *program;
    int matrix;
    int mvMatrix;
    int projMatrix;
    int normalMatrix;
    int worldMatrix;
    int texture0;
    int texture1;
    int texture2;
    int color;
    int numLights;
    int haveLight : 1;
    int haveLights : 1;
    int haveMaterial : 1;
    int haveMaterials : 1;

    void setUniformValue
        (const char *array, int index, const char *field, GLfloat v);
    void setUniformValue
        (const char *array, int index, const char *field, const QVector3D &v);
    void setUniformValue
        (const char *array, int index, const char *field, const QVector4D &v);
    void setUniformValue
        (const char *array, int index, const char *field, const QColor &v);

    void setLight
        (const QGLLightParameters *lparams, const QMatrix4x4 &ltransform,
         const char *array, int index);
    void setMaterial
        (const QGLMaterial *mparams, const QGLLightModel *model,
         const QGLLightParameters *lparams, const char *array, int index);
#endif
};

#if !defined(QGL_FIXED_FUNCTION_ONLY)

void QGLShaderProgramEffectPrivate::setUniformValue
    (const char *array, int index, const char *field, GLfloat v)
{
    char name[128];
    if (index >= 0)
        qsnprintf(name, sizeof(name), "%s[%d].%s", array, index, field);
    else
        qsnprintf(name, sizeof(name), "%s.%s", array, field);
    program->setUniformValue(name, v);
}

void QGLShaderProgramEffectPrivate::setUniformValue
    (const char *array, int index, const char *field, const QVector3D &v)
{
    char name[128];
    if (index >= 0)
        qsnprintf(name, sizeof(name), "%s[%d].%s", array, index, field);
    else
        qsnprintf(name, sizeof(name), "%s.%s", array, field);
    program->setUniformValue(name, v);
}

void QGLShaderProgramEffectPrivate::setUniformValue
    (const char *array, int index, const char *field, const QVector4D &v)
{
    char name[128];
    if (index >= 0)
        qsnprintf(name, sizeof(name), "%s[%d].%s", array, index, field);
    else
        qsnprintf(name, sizeof(name), "%s.%s", array, field);
    program->setUniformValue(name, v);
}

void QGLShaderProgramEffectPrivate::setUniformValue
    (const char *array, int index, const char *field, const QColor &v)
{
    char name[128];
    if (index >= 0)
        qsnprintf(name, sizeof(name), "%s[%d].%s", array, index, field);
    else
        qsnprintf(name, sizeof(name), "%s.%s", array, field);
    program->setUniformValue(name, v);
}

void QGLShaderProgramEffectPrivate::setLight
    (const QGLLightParameters *lparams, const QMatrix4x4 &ltransform,
     const char *array, int index)
{
    if (index >= 0) {
        // Single lights embed the color values into the material.
        setUniformValue(array, index, "ambient", lparams->ambientColor());
        setUniformValue(array, index, "diffuse", lparams->diffuseColor());
        setUniformValue(array, index, "specular", lparams->specularColor());
    }
    setUniformValue
        (array, index, "position", lparams->eyePosition(ltransform));
    setUniformValue
        (array, index, "spotDirection",
         lparams->eyeSpotDirection(ltransform).normalized());
    setUniformValue
        (array, index, "spotExponent", GLfloat(lparams->spotExponent()));
    setUniformValue
        (array, index, "spotCutoff", GLfloat(lparams->spotAngle()));
    setUniformValue
        (array, index, "spotCosCutoff", GLfloat(lparams->spotCosAngle()));
    setUniformValue
        (array, index, "constantAttenuation",
         GLfloat(lparams->constantAttenuation()));
    setUniformValue
        (array, index, "linearAttenuation",
         GLfloat(lparams->linearAttenuation()));
    setUniformValue
        (array, index, "quadraticAttenuation",
         GLfloat(lparams->quadraticAttenuation()));
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

void QGLShaderProgramEffectPrivate::setMaterial
    (const QGLMaterial *mparams, const QGLLightModel *model,
     const QGLLightParameters *lparams, const char *array, int index)
{
    if (lparams) {
        setUniformValue
            (array, index, "ambient",
             colorToVector4(mparams->ambientColor(), lparams->ambientColor()));
        setUniformValue
            (array, index, "diffuse",
             colorToVector4(mparams->diffuseColor(), lparams->diffuseColor()));
        setUniformValue
            (array, index, "specular",
             colorToVector4(mparams->specularColor(), lparams->specularColor()));
    } else {
        setUniformValue
            (array, index, "ambient", mparams->ambientColor());
        setUniformValue
            (array, index, "diffuse", mparams->diffuseColor());
        setUniformValue
            (array, index, "specular", mparams->specularColor());
    }
    setUniformValue
        (array, index, "emission",
         colorToVector4(mparams->emittedLight()) +
         colorToVector4(mparams->ambientColor(), model->ambientSceneColor()));
    setUniformValue
        (array, index, "shininess", GLfloat(mparams->shininess()));
}

#endif // !QGL_FIXED_FUNCTION_ONLY

/*!
    Constructs a new shader program effect.  This constructor is typically
    followed by calls to setVertexShader() and setFragmentShader().

    Note that a shader program effect will be bound to the QGLContext that
    is current when setActive() is called for the first time.  After that,
    the effect can only be used with that context or any other QGLContext
    that shares with it.
*/
QGLShaderProgramEffect::QGLShaderProgramEffect()
    : d_ptr(new QGLShaderProgramEffectPrivate)
{
}

/*!
    Destroys this shader program effect.
*/
QGLShaderProgramEffect::~QGLShaderProgramEffect()
{
}

/*!
    \reimp
*/
void QGLShaderProgramEffect::setActive(QGLPainter *painter, bool flag)
{
    Q_D(QGLShaderProgramEffect);

#if !defined(QGL_SHADERS_ONLY)
    d->fixedFunction = painter->isFixedFunction();
    if (d->fixedFunction) {
        // Fixed function emulation is flat color only.
        if (flag)
            glEnableClientState(GL_VERTEX_ARRAY);
        else
            glDisableClientState(GL_VERTEX_ARRAY);
        return;
    }
#endif

#if !defined(QGL_FIXED_FUNCTION_ONLY)
    static const char *const attributes[] = {
        "qt_Vertex",
        "qt_Normal",
        "qt_Color",
        "qt_MultiTexCoord0",
        "qt_MultiTexCoord1",
        "qt_MultiTexCoord2",
        "qt_Custom0",
        "qt_Custom1"
    };
    const int numAttributes = 8;
    Q_UNUSED(painter);
    int attr;
    if (d->regenerate) {
        // The shader source has changed since the last call to setActive().
        delete d->program;
        d->program = 0;
        d->regenerate = false;
    }
    if (!d->program) {
        if (!flag)
            return;
        if (d->vertexShader.isEmpty() || d->fragmentShader.isEmpty())
            return;

        d->program = new QGLShaderProgram();
        d->program->addShaderFromSourceCode
            (QGLShader::Vertex, d->vertexShader);
        d->program->addShaderFromSourceCode
            (QGLShader::Fragment, d->fragmentShader);
        if (!d->geometryShader.isEmpty())
        {
            d->program->addShaderFromSourceCode
                (QGLShader::Geometry, d->geometryShader);
            d->program->setGeometryInputType(d->geometryInputType);
            d->program->setGeometryOutputType(d->geometryOutputType);

        }

        if (beforeLink()) {
            for (attr = 0; attr < numAttributes; ++attr)
                d->program->bindAttributeLocation(attributes[attr], attr);
        }
        if (!d->program->link()) {
            qWarning("QGLShaderProgramEffect::setActive(): could not link shader program");
            delete d->program;
            d->program = 0;
            return;
        }
        afterLink();
        d->attributes = 0;
        for (attr = 0; attr < numAttributes; ++attr) {
            // Determine which attributes were actually present in the program.
            if (d->program->attributeLocation(attributes[attr]) != -1)
                d->attributes |= (1 << attr);
        }
        if (d->program->attributeLocation("qgl_Vertex") != -1)
            qWarning("QGLShaderProgramEffect: qgl_Vertex no longer supported; use qt_Vertex instead");
        d->matrix = d->program->uniformLocation("qt_ModelViewProjectionMatrix");
        d->mvMatrix = d->program->uniformLocation("qt_ModelViewMatrix");
        d->projMatrix = d->program->uniformLocation("qt_ProjectionMatrix");
        d->normalMatrix = d->program->uniformLocation("qt_NormalMatrix");
        d->worldMatrix = d->program->uniformLocation("qt_WorldMatrix");
        d->texture0 = d->program->uniformLocation("qt_Texture0");
        d->texture1 = d->program->uniformLocation("qt_Texture1");
        d->texture2 = d->program->uniformLocation("qt_Texture2");
        d->color = d->program->uniformLocation("qt_Color");
        d->numLights = d->program->uniformLocation("qt_NumLights");
        d->haveLight =
            (d->program->uniformLocation("qt_Light.position") != -1);
        d->haveLights =
            (d->program->uniformLocation("qt_Lights[0].position") != -1);
        d->haveMaterial =
                (d->program->uniformLocation("qt_Material.ambient") != -1) ||
                (d->program->uniformLocation("qt_Material.diffuse") != -1) ||
                (d->program->uniformLocation("qt_Material.specular") != -1) ||
                (d->program->uniformLocation("qt_Material.emission") != -1);
        d->haveMaterials =
            (d->program->uniformLocation("qt_Material[0].ambient") != -1) ||
            (d->program->uniformLocation("qt_Material[0].diffuse") != -1) ||
            (d->program->uniformLocation("qt_Material[0].specular") != -1) ||
            (d->program->uniformLocation("qt_Material[0].emission") != -1);
    }
    if (flag) {
        d->program->bind();
        for (attr = 0; attr < numAttributes; ++attr) {
            if ((d->attributes & (1 << attr)) == 0)
                continue;
            d->program->enableAttributeArray(attr);
        }
        if (d->texture0 != -1)
            d->program->setUniformValue(d->texture0, 0);
        if (d->texture1 != -1)
            d->program->setUniformValue(d->texture1, 1);
        if (d->texture2 != -1)
            d->program->setUniformValue(d->texture2, 2);
    } else {
        for (attr = 0; attr < int(QGL::UserVertex); ++attr) {
            if ((d->attributes & (1 << attr)) != 0)
                d->program->disableAttributeArray(attr);
        }
        d->program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLShaderProgramEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    Q_D(QGLShaderProgramEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->fixedFunction) {
        // Fixed function emulation is flat color only.
        painter->updateFixedFunction
            (updates & (QGLPainter::UpdateColor | QGLPainter::UpdateMatrices));
        return;
    }
#endif
#if !defined(QGL_FIXED_FUNCTION_ONLY)
    if ((updates & QGLPainter::UpdateColor) != 0 && d->color != -1)
        d->program->setUniformValue(d->color, painter->color());
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        if (d->matrix != -1)
            d->program->setUniformValue(d->matrix, painter->combinedMatrix());
    }
    if ((updates & QGLPainter::UpdateModelViewMatrix) != 0) {
        if (d->mvMatrix != -1)
            d->program->setUniformValue(d->mvMatrix, painter->modelViewMatrix());
        if (d->normalMatrix != -1)
            d->program->setUniformValue(d->normalMatrix, painter->normalMatrix());
        if (d->worldMatrix != -1)
            d->program->setUniformValue(d->worldMatrix, painter->worldMatrix());
    }
    if ((updates & QGLPainter::UpdateProjectionMatrix) != 0) {
        if (d->projMatrix != -1)
            d->program->setUniformValue(d->projMatrix, painter->projectionMatrix());
    }
    if ((updates & QGLPainter::UpdateLights) != 0) {
        if (d->haveLight) {
            // Only one light needed so make it the main light.
            d->setLight(painter->mainLight(), painter->mainLightTransform(),
                        "qt_Light", -1);
        } else if (d->haveLights) {
            // Shader supports multiple light sources.
            int numLights = 0;
            int maxLightId = painter->maximumLightId();
            if (maxLightId < 0) {
                // No lights - re-enable the main light so we have something.
                painter->mainLight();
                maxLightId = 0;
            }
            for (int lightId = 0; lightId <= maxLightId; ++lightId) {
                // Is this light currently enabled?
                const QGLLightParameters *lparams = painter->light(lightId);
                if (!lparams)
                    continue;

                // Set the parameters for the next shader light number.
                d->setLight(lparams, painter->lightTransform(lightId),
                            "qt_Lights", numLights);

                // Bail out if we've hit the maximum shader light limit.
                ++numLights;
                if (numLights >= d->maximumLights)
                    break;
            }
            if (d->numLights != -1)
                d->program->setUniformValue(d->numLights, numLights);
        }
    }
    if ((updates & QGLPainter::UpdateMaterials) != 0 ||
            ((updates & QGLPainter::UpdateLights) != 0 && d->haveLight)) {
        if (d->haveLight) {
            // For a single light source, combine the light colors
            // into the material colors.
            if (d->haveMaterial) {
                d->setMaterial(painter->faceMaterial(QGL::FrontFaces),
                               painter->lightModel(), painter->mainLight(),
                               "qt_Material", -1);
            } else if (d->haveMaterials) {
                d->setMaterial(painter->faceMaterial(QGL::FrontFaces),
                               painter->lightModel(), painter->mainLight(),
                               "qt_Materials", 0);
                d->setMaterial(painter->faceMaterial(QGL::BackFaces),
                               painter->lightModel(), painter->mainLight(),
                               "qt_Materials", 1);
            }
        } else {
            // Multiple light sources, so light colors are separate.
            if (d->haveMaterial) {
                d->setMaterial(painter->faceMaterial(QGL::FrontFaces),
                               painter->lightModel(), 0, "qt_Material", -1);
            } else if (d->haveMaterials) {
                d->setMaterial(painter->faceMaterial(QGL::FrontFaces),
                               painter->lightModel(), 0, "qt_Materials", 0);
                d->setMaterial(painter->faceMaterial(QGL::BackFaces),
                               painter->lightModel(), 0, "qt_Materials", 1);
            }
        }
    }
#endif
}

/*!
    Returns the source code for the vertex shader.

    \sa setVertexShader(), geometryShader(), fragmentShader(), setVertexShaderFromFile()
*/
QByteArray QGLShaderProgramEffect::vertexShader() const
{
    Q_D(const QGLShaderProgramEffect);
    return d->vertexShader;
}

/*!
    Sets the \a source code for the vertex shader.

    \sa vertexShader(), setGeometryShader(), setFragmentShader(), setVertexShaderFromFile()
*/
void QGLShaderProgramEffect::setVertexShader(const QByteArray &source)
{
    Q_D(QGLShaderProgramEffect);
    d->vertexShader = source;
    d->regenerate = true;
}

/*!
    Sets the source code for the vertex shader to the contents
    of \a fileName.

    \sa setVertexShader(), setGeometryShaderFromFile(), setFragmentShaderFromFile()
*/
void QGLShaderProgramEffect::setVertexShaderFromFile(const QString &fileName)
{
    Q_D(QGLShaderProgramEffect);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        d->vertexShader = file.readAll();
        d->regenerate = true;
    } else {
        qWarning() << "QGLShaderProgramEffect::setVertexShaderFromFile: could not open " << fileName;
    }
}

/*!
    Returns the source code for the geometry shader.

    \sa setGeometryShader(), fragmentShader(), setGeometryShaderFromFile()
*/
QByteArray QGLShaderProgramEffect::geometryShader() const
{
    Q_D(const QGLShaderProgramEffect);
    return d->geometryShader;
}

/*!
    Sets the \a source code for the geometry shader.

    \sa geometryShader(), setFragmentShader(), setGeometryShaderFromFile()
*/
void QGLShaderProgramEffect::setGeometryShader(const QByteArray &source)
{
    Q_D(QGLShaderProgramEffect);
    d->geometryShader = source;
    d->regenerate = true;
}

/*!
    Sets the source code for the geometry shader to the contents
    of \a fileName.

    \sa setGeometryShader(), setFragmentShaderFromFile()
*/
void QGLShaderProgramEffect::setGeometryShaderFromFile(const QString &fileName)
{
    Q_D(QGLShaderProgramEffect);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        d->geometryShader = file.readAll();
        d->regenerate = true;
    } else {
        qWarning() << "QGLShaderProgramEffect::setGeometryShaderFromFile: could not open " << fileName;
    }
}

/*!
    Returns the source code for the fragment shader.

    \sa setFragmentShader(), vertexShader(), geometryShader()
*/
QByteArray QGLShaderProgramEffect::fragmentShader() const
{
    Q_D(const QGLShaderProgramEffect);
    return d->fragmentShader;
}

/*!
    Sets the source code for the fragment shader to the contents
    of \a fileName.

    \sa setFragmentShader(), setVertexShaderFromFile(), setGeometryShaderFromFile()
*/
void QGLShaderProgramEffect::setFragmentShaderFromFile(const QString &fileName)
{
    Q_D(QGLShaderProgramEffect);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        d->fragmentShader = file.readAll();
        d->regenerate = true;
    } else {
        qWarning() << "QGLShaderProgramEffect::setFragmentShaderFromFile: could not open " << fileName;
    }
}

/*!
    Sets the \a source code for the fragment shader.

    \sa fragmentShader(), setVertexShader(), setGeometryShader()
*/
void QGLShaderProgramEffect::setFragmentShader(const QByteArray &source)
{
    Q_D(QGLShaderProgramEffect);
    d->fragmentShader = source;
    d->regenerate = true;
}

/*!
    Sets the type of primitive the program's geometry shader is expecting to
    recieve from the vertex shader to \a drawingMode.  The default value is
    GL_TRIANGLE_STRIP.

    If the program has no geometry shader, this has no effect.
*/
void QGLShaderProgramEffect::setGeometryInputType(GLenum drawingMode)
{
    Q_D(QGLShaderProgramEffect);
    d->geometryInputType = drawingMode;
}

/*!
    Sets what sort of primitives the program's geometry shader will produce
    to \a drawingMode. The default value is GL_TRIANGLE_STRIP.

    If the program has no geometry shader, this has no effect.
*/
void QGLShaderProgramEffect::setGeometryOutputType(GLenum drawingMode)
{
    Q_D(QGLShaderProgramEffect);
    d->geometryOutputType = drawingMode;
}

/*!
    Returns the type of primitives this program's geometry shader is expecting.
*/
GLenum QGLShaderProgramEffect::geometryInputType()
{
    Q_D(QGLShaderProgramEffect);
    return d->geometryInputType;
}

/*!
    Returns the type of primitive this program's geometry shader will produce.
*/
GLenum QGLShaderProgramEffect::geometryOutputType()
{
    Q_D(QGLShaderProgramEffect);
    return d->geometryOutputType;
}

/*!
    Returns the maximum number of lights that are supported by this
    shader program effect.  The default value is 8.

    The actual number of lights will be provided to the vertexShader()
    as the \c{qt_NumLights} uniform variable, which will always be
    less than or equal to maximumLights().

    \sa setMaximumLights()
*/
int QGLShaderProgramEffect::maximumLights() const
{
    Q_D(const QGLShaderProgramEffect);
    return d->maximumLights;
}

/*!
    Sets the maximum number of lights that are supported by this
    shader program effect to \a value.

    \sa maximumLights()
*/
void QGLShaderProgramEffect::setMaximumLights(int value)
{
    Q_D(QGLShaderProgramEffect);
    d->maximumLights = value;
}

/*!
    Returns the shader program object that was created for this effect;
    null if setActive() has not been called yet.

    This function can be used by the application to adjust custom
    uniform variables after the effect is activated on a QGLPainter:

    \code
    painter.setUserEffect(effect);
    effect->program()->setUniformValue("springiness", GLfloat(0.5f));
    \endcode
*/
QGLShaderProgram *QGLShaderProgramEffect::program() const
{
#if !defined(QGL_FIXED_FUNCTION_ONLY)
    Q_D(const QGLShaderProgramEffect);
    return d->program;
#else
    return 0;
#endif
}

/*!
    Called by setActive() just before the program() is linked.
    Returns true if the standard vertex attributes should be bound
    by calls to QGLShaderProgram::bindAttributeLocation().  Returns
    false if the subclass has already bound the attributes.

    This function can be overridden by subclasses to alter the
    vertex attribute bindings, or to add additional shader stages
    to program().

    \sa afterLink()
*/
bool QGLShaderProgramEffect::beforeLink()
{
    return true;
}

/*!
    Called by setActive() just after the program() is linked.
    The default implementation does nothing.

    This function can be overridden by subclasses to resolve uniform
    variable locations and cache them for later use in update().

    \sa beforeLink()
*/
void QGLShaderProgramEffect::afterLink()
{
}

QT_END_NAMESPACE
