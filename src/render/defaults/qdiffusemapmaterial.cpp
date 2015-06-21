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

#include "qdiffusemapmaterial.h"
#include "qdiffusemapmaterial_p.h"
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qopenglfilter.h>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QDiffuseMapMaterialPrivate
    \internal
*/
QDiffuseMapMaterialPrivate::QDiffuseMapMaterialPrivate()
    : QMaterialPrivate()
    , m_diffuseMapEffect(new QEffect())
    , m_diffuseTexture(new QTexture2D())
    , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
    , m_diffuseParameter(new QParameter(QStringLiteral("diffuseTexture"), m_diffuseTexture))
    , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
    , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
    , m_lightPositionParameter(new QParameter(QStringLiteral("lightPosition"), QVector4D(0.0f, 0.0f, 0.0f, 1.0f)))
    , m_lightIntensityParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
    , m_textureScaleParameter(new QParameter(QStringLiteral("texCoordScale"), 1.0f))
    , m_diffuseMapGL3Technique(new QTechnique())
    , m_diffuseMapGL2Technique(new QTechnique())
    , m_diffuseMapES2Technique(new QTechnique())
    , m_diffuseMapGL3RenderPass(new QRenderPass())
    , m_diffuseMapGL2RenderPass(new QRenderPass())
    , m_diffuseMapES2RenderPass(new QRenderPass())
    , m_diffuseMapGL3Shader(new QShaderProgram())
    , m_diffuseMapGL2ES2Shader(new QShaderProgram())
 {
    m_diffuseTexture->setMagnificationFilter(QAbstractTextureProvider::Linear);
    m_diffuseTexture->setMinificationFilter(QAbstractTextureProvider::LinearMipMapLinear);
    m_diffuseTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::Repeat));
    m_diffuseTexture->setGenerateMipMaps(true);
    m_diffuseTexture->setMaximumAnisotropy(16.0f);
}

void QDiffuseMapMaterialPrivate::init()
{
    m_diffuseMapGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/diffusemap.vert"))));
    m_diffuseMapGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/diffusemap.frag"))));
    m_diffuseMapGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/diffusemap.vert"))));
    m_diffuseMapGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/diffusemap.frag"))));

    m_diffuseMapGL3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_diffuseMapGL3Technique->openGLFilter()->setMajorVersion(3);
    m_diffuseMapGL3Technique->openGLFilter()->setMinorVersion(1);
    m_diffuseMapGL3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_diffuseMapGL2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_diffuseMapGL2Technique->openGLFilter()->setMajorVersion(2);
    m_diffuseMapGL2Technique->openGLFilter()->setMinorVersion(0);
    m_diffuseMapGL2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_diffuseMapES2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_diffuseMapES2Technique->openGLFilter()->setMajorVersion(2);
    m_diffuseMapES2Technique->openGLFilter()->setMinorVersion(0);
    m_diffuseMapES2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_diffuseMapGL3RenderPass->setShaderProgram(m_diffuseMapGL3Shader);
    m_diffuseMapGL2RenderPass->setShaderProgram(m_diffuseMapGL2ES2Shader);
    m_diffuseMapES2RenderPass->setShaderProgram(m_diffuseMapGL2ES2Shader);

    m_diffuseMapGL3Technique->addPass(m_diffuseMapGL3RenderPass);
    m_diffuseMapGL2Technique->addPass(m_diffuseMapGL2RenderPass);
    m_diffuseMapES2Technique->addPass(m_diffuseMapES2RenderPass);

    m_diffuseMapEffect->addTechnique(m_diffuseMapGL3Technique);
    m_diffuseMapEffect->addTechnique(m_diffuseMapGL2Technique);
    m_diffuseMapEffect->addTechnique(m_diffuseMapES2Technique);

    m_diffuseMapEffect->addParameter(m_ambientParameter);
    m_diffuseMapEffect->addParameter(m_diffuseParameter);
    m_diffuseMapEffect->addParameter(m_specularParameter);
    m_diffuseMapEffect->addParameter(m_shininessParameter);
    m_diffuseMapEffect->addParameter(m_lightPositionParameter);
    m_diffuseMapEffect->addParameter(m_lightIntensityParameter);
    m_diffuseMapEffect->addParameter(m_textureScaleParameter);

    q_func()->setEffect(m_diffuseMapEffect);
}

/*!
    \class Qt3D::QDiffuseMapMaterial
    \brief The QDiffuseMapMaterial provides a default implementation of the phong lighting effect where the diffuse light component
    is read from a texture map.
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
    Constructs a new Qt3D::QDiffuseMapMaterial instance with parent object \a parent.
 */

QDiffuseMapMaterial::QDiffuseMapMaterial(QNode *parent)
    : QMaterial(*new QDiffuseMapMaterialPrivate, parent)
{
    Q_D(QDiffuseMapMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    QObject::connect(d->m_textureScaleParameter, SIGNAL(valueChanged()), this, SIGNAL(textureScaleChanged()));
    d->init();
}

/*!
    Destroys the QDiffuseMapMaterial instance.
*/
QDiffuseMapMaterial::~QDiffuseMapMaterial()
{
}

/*!
    \property Qt3D::QDiffuseMapMaterial::ambient

    Holds the current ambient color.
 */
QColor QDiffuseMapMaterial::ambient() const
{
    Q_D(const QDiffuseMapMaterial);
    return d->m_ambientParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QDiffuseMapMaterial::specular

    Holds the current specular color.
 */
QColor QDiffuseMapMaterial::specular() const
{
    Q_D(const QDiffuseMapMaterial);
    return d->m_specularParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QDiffuseMapMaterial::shininess

    Holds the current shininess as a float value.
 */
float QDiffuseMapMaterial::shininess() const
{
    Q_D(const QDiffuseMapMaterial);
    return d->m_diffuseParameter->value().toFloat();
}

/*!
    \property Qt3D::QDiffuseMapMaterial::diffuse

    Holds the current QTexture used as the diffuse map.

    By default, the diffuse texture has the following properties:

    \list
        \li Linear minification and magnification filters
        \li Linear mipmap with mipmapping enabled
        \li Repeat wrap mode
        \li Maximum anisotropy of 16.0
    \endlist
 */
QAbstractTextureProvider *QDiffuseMapMaterial::diffuse() const
{
    Q_D(const QDiffuseMapMaterial);
    return d->m_diffuseParameter->value().value<QAbstractTextureProvider *>();
}

/*!
    \property Qt3D::QDiffuseMapMaterial::textureScale

    Holds the current texture scale as a float value.
 */
float QDiffuseMapMaterial::textureScale() const
{
    Q_D(const QDiffuseMapMaterial);
    return d->m_textureScaleParameter->value().toFloat();
}

void QDiffuseMapMaterial::setAmbient(const QColor &ambient)
{
    Q_D(const QDiffuseMapMaterial);
    d->m_ambientParameter->setValue(ambient);
}

void QDiffuseMapMaterial::setSpecular(const QColor &specular)
{
    Q_D(QDiffuseMapMaterial);
    d->m_specularParameter->setValue(specular);
}

void QDiffuseMapMaterial::setShininess(float shininess)
{
    Q_D(QDiffuseMapMaterial);
    d->m_shininessParameter->setValue(shininess);
}

void QDiffuseMapMaterial::setDiffuse(QAbstractTextureProvider *diffuseMap)
{
    Q_D(QDiffuseMapMaterial);
    d->m_diffuseParameter->setValue(QVariant::fromValue(diffuseMap));
}

void QDiffuseMapMaterial::setTextureScale(float textureScale)
{
    Q_D(QDiffuseMapMaterial);
    d->m_textureScaleParameter->setValue(textureScale);
}

} // Qt3D

QT_END_NAMESPACE
