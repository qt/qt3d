/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnormaldiffusemapmaterial.h"
#include "qnormaldiffusemapmaterial_p.h"

#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qopenglfilter.h>

#include <QUrl>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QNormalDiffuseMapMaterialPrivate
    \internal
*/
QNormalDiffuseMapMaterialPrivate::QNormalDiffuseMapMaterialPrivate()
    : QMaterialPrivate()
    , m_normalDiffuseEffect(new QEffect())
    , m_diffuseTexture(new QTexture2D())
    , m_normalTexture(new QTexture2D())
    , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.1f, 0.1f, 0.1f, 1.0f)))
    , m_diffuseParameter(new QParameter(QStringLiteral("diffuseTexture"), m_diffuseTexture))
    , m_normalParameter(new QParameter(QStringLiteral("normalTexture"), m_normalTexture))
    , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
    , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
    , m_lightPositionParameter(new QParameter(QStringLiteral("lightPosition"), QVector4D(0.0f, 0.0f, 0.0f, 1.0f)))
    , m_lightIntensityParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
    , m_textureScaleParameter(new QParameter(QStringLiteral("texCoordScale"), 1.0f))
    , m_normalDiffuseGL3Technique(new QTechnique())
    , m_normalDiffuseGL2Technique(new QTechnique())
    , m_normalDiffuseES2Technique(new QTechnique())
    , m_normalDiffuseGL3RenderPass(new QRenderPass())
    , m_normalDiffuseGL2RenderPass(new QRenderPass())
    , m_normalDiffuseES2RenderPass(new QRenderPass())
    , m_normalDiffuseGL3Shader(new QShaderProgram())
    , m_normalDiffuseGL2ES2Shader(new QShaderProgram())
{
    m_diffuseTexture->setMagnificationFilter(QAbstractTextureProvider::Linear);
    m_diffuseTexture->setMinificationFilter(QAbstractTextureProvider::LinearMipMapLinear);
    m_diffuseTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::Repeat));
    m_diffuseTexture->setGenerateMipMaps(true);
    m_diffuseTexture->setMaximumAnisotropy(16.0f);

    m_normalTexture->setMagnificationFilter(QAbstractTextureProvider::Linear);
    m_normalTexture->setMinificationFilter(QAbstractTextureProvider::Linear);
    m_normalTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::Repeat));
    m_normalTexture->setMaximumAnisotropy(16.0f);
}

void QNormalDiffuseMapMaterialPrivate::init()
{
    m_normalDiffuseGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/normaldiffusemap.vert"))));
    m_normalDiffuseGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/normaldiffusemap.frag"))));
    m_normalDiffuseGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/normaldiffusemap.vert"))));
    m_normalDiffuseGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/normaldiffusemap.frag"))));

    m_normalDiffuseGL3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_normalDiffuseGL3Technique->openGLFilter()->setMajorVersion(3);
    m_normalDiffuseGL3Technique->openGLFilter()->setMinorVersion(1);
    m_normalDiffuseGL3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_normalDiffuseGL2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_normalDiffuseGL2Technique->openGLFilter()->setMajorVersion(2);
    m_normalDiffuseGL2Technique->openGLFilter()->setMinorVersion(0);
    m_normalDiffuseGL2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_normalDiffuseES2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_normalDiffuseES2Technique->openGLFilter()->setMajorVersion(2);
    m_normalDiffuseES2Technique->openGLFilter()->setMinorVersion(0);
    m_normalDiffuseES2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_normalDiffuseGL3RenderPass->setShaderProgram(m_normalDiffuseGL3Shader);
    m_normalDiffuseGL2RenderPass->setShaderProgram(m_normalDiffuseGL2ES2Shader);
    m_normalDiffuseES2RenderPass->setShaderProgram(m_normalDiffuseGL2ES2Shader);

    m_normalDiffuseGL3Technique->addPass(m_normalDiffuseGL3RenderPass);
    m_normalDiffuseGL2Technique->addPass(m_normalDiffuseGL2RenderPass);
    m_normalDiffuseES2Technique->addPass(m_normalDiffuseES2RenderPass);

    m_normalDiffuseEffect->addTechnique(m_normalDiffuseGL3Technique);
    m_normalDiffuseEffect->addTechnique(m_normalDiffuseGL2Technique);
    m_normalDiffuseEffect->addTechnique(m_normalDiffuseES2Technique);

    m_normalDiffuseEffect->addParameter(m_ambientParameter);
    m_normalDiffuseEffect->addParameter(m_diffuseParameter);
    m_normalDiffuseEffect->addParameter(m_normalParameter);
    m_normalDiffuseEffect->addParameter(m_specularParameter);
    m_normalDiffuseEffect->addParameter(m_shininessParameter);
    m_normalDiffuseEffect->addParameter(m_lightPositionParameter);
    m_normalDiffuseEffect->addParameter(m_lightIntensityParameter);
    m_normalDiffuseEffect->addParameter(m_textureScaleParameter);

    q_func()->setEffect(m_normalDiffuseEffect);
}

/*!
    \class Qt3D::QNormalDiffuseMapMaterial
    \brief The QNormalDiffuseMapMaterial provides a default implementation of the phong lighting and bump effect where the diffuse light component
    is read from a texture map and the normals of the mesh being rendered from a normal texture map.
    \inmodule Qt3DRenderer
    \since 5.5

    The specular lighting effect is based on the combination of 3 lighting components ambient, diffuse and specular.
    The relative strengths of these components is controlled by means of their reflectivity coefficients which are modelled as RGB triplets:

    \list
    \li Ambient is the color that is emitted by an object without any other light source.
    \li Diffuse is the color that is emitted for rought surface reflections with the lights.
    \li Specular is the color emitted for shiny surface reflections with the lights.
    \li The shininess of a surface is controlled by a float property.
    \endlist

    This material uses an effect with a single render pass approach and performs per fragment lighting.
    Techniques are provided for OpenGL 2, OpenGL 3 or above as well as OpenGL ES 2.
*/

/*!
    Constructs a new Qt3D::QNormalDiffuseMapMaterial instance with parent object \a parent.
*/
QNormalDiffuseMapMaterial::QNormalDiffuseMapMaterial(QNode *parent)
    : QMaterial(*new QNormalDiffuseMapMaterialPrivate, parent)
{
    Q_D(QNormalDiffuseMapMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_normalParameter, SIGNAL(valueChanged()), this, SIGNAL(normalChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    QObject::connect(d->m_textureScaleParameter, SIGNAL(valueChanged()), this, SIGNAL(textureScaleChanged()));
    d->init();
}

/*! \internal */
QNormalDiffuseMapMaterial::QNormalDiffuseMapMaterial(QNormalDiffuseMapMaterialPrivate &dd, QNode *parent)
    : QMaterial(dd, parent)
{
    Q_D(QNormalDiffuseMapMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_normalParameter, SIGNAL(valueChanged()), this, SIGNAL(normalChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    QObject::connect(d->m_textureScaleParameter, SIGNAL(valueChanged()), this, SIGNAL(textureScaleChanged()));
    d->init();
}

/*!
    Destroys the QNormalDiffuseMapMaterial instance.
*/
QNormalDiffuseMapMaterial::~QNormalDiffuseMapMaterial()
{
}

/*!
    \property Qt3D::QNormalDiffuseMapMaterial::ambient
    Holds the current ambient color.
 */
QColor QNormalDiffuseMapMaterial::ambient() const
{
    Q_D(const QNormalDiffuseMapMaterial);
    return d->m_ambientParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QNormalDiffuseMapMaterial::specular
    Holds the current specular color.
 */
QColor QNormalDiffuseMapMaterial::specular() const
{
    Q_D(const QNormalDiffuseMapMaterial);
    return d->m_specularParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QNormalDiffuseMapMaterial::diffuse

    Holds the current diffuse map texture.

    By default, the diffuse texture has these properties:

    \list
        \li Linear minification and magnification filters
        \li Linear mipmap with mipmapping enabled
        \li Repeat wrap modeM
        \li Maximum anisotropy of 16.0
    \endlist
 */
QAbstractTextureProvider *QNormalDiffuseMapMaterial::diffuse() const
{
    Q_D(const QNormalDiffuseMapMaterial);
    return d->m_diffuseParameter->value().value<QAbstractTextureProvider *>();
}

/*!
    \property Qt3D::QNormalDiffuseMapMaterial::normal

    Holds the current normal map texture.

    By default, the normal texture has the following properties:

    \list
        \li Linear minification and magnification filters
        \li Repeat wrap mode
        \li Maximum anisotropy of 16.0
    \endlist
 */
QAbstractTextureProvider *QNormalDiffuseMapMaterial::normal() const
{
    Q_D(const QNormalDiffuseMapMaterial);
    return d->m_normalParameter->value().value<QAbstractTextureProvider *>();
}

/*!
    \property Qt3D::QNormalDiffuseMapMaterial::shininess

    Holds the current shininess as a float value.
 */
float QNormalDiffuseMapMaterial::shininess() const
{
    Q_D(const QNormalDiffuseMapMaterial);
    return d->m_shininessParameter->value().toFloat();
}

/*!
    \property Qt3D::QNormalDiffuseMapMaterial::textureScale

    Holds the current texture scale as a float value.
 */
float QNormalDiffuseMapMaterial::textureScale() const
{
    Q_D(const QNormalDiffuseMapMaterial);
    return d->m_textureScaleParameter->value().toFloat();
}

void QNormalDiffuseMapMaterial::setAmbient(const QColor &ambient)
{
    Q_D(QNormalDiffuseMapMaterial);
    d->m_ambientParameter->setValue(ambient);
}

void QNormalDiffuseMapMaterial::setSpecular(const QColor &specular)
{
    Q_D(QNormalDiffuseMapMaterial);
    d->m_specularParameter->setValue(specular);
}

void QNormalDiffuseMapMaterial::setDiffuse(QAbstractTextureProvider *diffuse)
{
    Q_D(QNormalDiffuseMapMaterial);
    d->m_diffuseParameter->setValue(QVariant::fromValue(diffuse));
}

void QNormalDiffuseMapMaterial::setNormal(QAbstractTextureProvider *normal)
{
    Q_D(QNormalDiffuseMapMaterial);
    d->m_normalParameter->setValue(QVariant::fromValue(normal));
}

void QNormalDiffuseMapMaterial::setShininess(float shininess)
{
    Q_D(QNormalDiffuseMapMaterial);
    d->m_shininessParameter->setValue(shininess);
}

void QNormalDiffuseMapMaterial::setTextureScale(float textureScale)
{
    Q_D(QNormalDiffuseMapMaterial);
    d->m_textureScaleParameter->setValue(textureScale);
}

} // Qt3D

QT_END_NAMESPACE
