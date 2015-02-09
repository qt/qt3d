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

#include "qnormaldiffusespecularmapmaterial.h"
#include "qnormaldiffusespecularmapmaterial_p.h"
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

QNormalDiffuseSpecularMapMaterialPrivate::QNormalDiffuseSpecularMapMaterialPrivate(QNormalDiffuseSpecularMapMaterial *qq)
    : QMaterialPrivate(qq)
    , m_normalDiffuseSpecularEffect(new QEffect())
    , m_diffuseTexture(new QTexture2D())
    , m_normalTexture(new QTexture2D())
    , m_specularTexture(new QTexture2D())
    , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
    , m_diffuseParameter(new QParameter(QStringLiteral("diffuseTexture"), m_diffuseTexture))
    , m_normalParameter(new QParameter(QStringLiteral("normalTexture"), m_normalTexture))
    , m_specularParameter(new QParameter(QStringLiteral("specularTexture"), m_specularTexture))
    , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
    , m_lightPositionParameter(new QParameter(QStringLiteral("lightPosition"), QVector4D(0.0f, 0.0f, 0.0f, 1.0f)))
    , m_lightIntensityParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
    , m_textureScaleParameter(new QParameter(QStringLiteral("texCoordScale"), 1.0f))
    , m_normalDiffuseSpecularGL3Technique(new QTechnique())
    , m_normalDiffuseSpecularGL2Technique(new QTechnique())
    , m_normalDiffuseSpecularES2Technique(new QTechnique())
    , m_normalDiffuseSpecularGL3RenderPass(new QRenderPass())
    , m_normalDiffuseSpecularGL2RenderPass(new QRenderPass())
    , m_normalDiffuseSpecularES2RenderPass(new QRenderPass())
    , m_normalDiffuseSpecularGL3Shader(new QShaderProgram())
    , m_normalDiffuseSpecularGL2ES2Shader(new QShaderProgram())
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

    m_specularTexture->setMagnificationFilter(QAbstractTextureProvider::Linear);
    m_specularTexture->setMinificationFilter(QAbstractTextureProvider::LinearMipMapLinear);
    m_specularTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::Repeat));
    m_specularTexture->setGenerateMipMaps(true);
    m_specularTexture->setMaximumAnisotropy(16.0f);
}

void QNormalDiffuseSpecularMapMaterialPrivate::init()
{
    m_normalDiffuseSpecularGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/normaldiffusespecularmap.vert"))));
    m_normalDiffuseSpecularGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/normaldiffusespecularmap.frag"))));
    m_normalDiffuseSpecularGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/normaldiffusespecularmap.vert"))));
    m_normalDiffuseSpecularGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/normaldiffusespecularmap.frag"))));

    m_normalDiffuseSpecularGL3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_normalDiffuseSpecularGL3Technique->openGLFilter()->setMajorVersion(3);
    m_normalDiffuseSpecularGL3Technique->openGLFilter()->setMinorVersion(1);
    m_normalDiffuseSpecularGL3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_normalDiffuseSpecularGL2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_normalDiffuseSpecularGL2Technique->openGLFilter()->setMajorVersion(2);
    m_normalDiffuseSpecularGL2Technique->openGLFilter()->setMinorVersion(0);
    m_normalDiffuseSpecularGL2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_normalDiffuseSpecularES2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_normalDiffuseSpecularES2Technique->openGLFilter()->setMajorVersion(2);
    m_normalDiffuseSpecularES2Technique->openGLFilter()->setMinorVersion(0);
    m_normalDiffuseSpecularES2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_normalDiffuseSpecularGL3RenderPass->setShaderProgram(m_normalDiffuseSpecularGL3Shader);
    m_normalDiffuseSpecularGL2RenderPass->setShaderProgram(m_normalDiffuseSpecularGL2ES2Shader);
    m_normalDiffuseSpecularES2RenderPass->setShaderProgram(m_normalDiffuseSpecularGL2ES2Shader);

    m_normalDiffuseSpecularGL3Technique->addPass(m_normalDiffuseSpecularGL3RenderPass);
    m_normalDiffuseSpecularGL2Technique->addPass(m_normalDiffuseSpecularGL2RenderPass);
    m_normalDiffuseSpecularES2Technique->addPass(m_normalDiffuseSpecularES2RenderPass);

    m_normalDiffuseSpecularEffect->addTechnique(m_normalDiffuseSpecularGL3Technique);
    m_normalDiffuseSpecularEffect->addTechnique(m_normalDiffuseSpecularGL2Technique);
    m_normalDiffuseSpecularEffect->addTechnique(m_normalDiffuseSpecularES2Technique);

    m_normalDiffuseSpecularEffect->addParameter(m_ambientParameter);
    m_normalDiffuseSpecularEffect->addParameter(m_diffuseParameter);
    m_normalDiffuseSpecularEffect->addParameter(m_normalParameter);
    m_normalDiffuseSpecularEffect->addParameter(m_specularParameter);
    m_normalDiffuseSpecularEffect->addParameter(m_shininessParameter);
    m_normalDiffuseSpecularEffect->addParameter(m_lightPositionParameter);
    m_normalDiffuseSpecularEffect->addParameter(m_lightIntensityParameter);
    m_normalDiffuseSpecularEffect->addParameter(m_textureScaleParameter);

    static_cast<QMaterial *>(q_ptr)->setEffect(m_normalDiffuseSpecularEffect);
}

/*!
    \class Qt3D::QNormalDiffuseSpecularMapMaterial
    \brief The QNormalDiffuseSpecularMapMaterial provides a default implementation of the phong lighting and bump effect where the diffuse and specular light components
    are read from texture maps and the normals of the mesh being rendered from a normal texture map.
    \inmodule Qt3DRender
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
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::QNormalDiffuseSpecularMapMaterial()
    Constructs a new Qt3D::QNormalDiffuseSpecularMapMaterial instance with parent object \a parent.
*/
QNormalDiffuseSpecularMapMaterial::QNormalDiffuseSpecularMapMaterial(QNode *parent)
    : QMaterial(*new QNormalDiffuseSpecularMapMaterialPrivate(this), parent)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_normalParameter, SIGNAL(valueChanged()), this, SIGNAL(normalChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    QObject::connect(d->m_textureScaleParameter, SIGNAL(valueChanged()), this, SIGNAL(textureScaleChanged()));
    d->init();
}


QNormalDiffuseSpecularMapMaterial::QNormalDiffuseSpecularMapMaterial(QNormalDiffuseSpecularMapMaterialPrivate &dd, QNode *parent)
    : QMaterial(dd, parent)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_normalParameter, SIGNAL(valueChanged()), this, SIGNAL(normalChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    QObject::connect(d->m_textureScaleParameter, SIGNAL(valueChanged()), this, SIGNAL(textureScaleChanged()));
    d->init();
}

QNormalDiffuseSpecularMapMaterial::~QNormalDiffuseSpecularMapMaterial()
{
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::ambient() const
    \return the current QColor ambient value.
 */
QColor QNormalDiffuseSpecularMapMaterial::ambient() const
{
    Q_D(const QNormalDiffuseSpecularMapMaterial);
    return d->m_ambientParameter->value().value<QColor>();
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::diffuse() const
    \return the current diffuse map texture.
    \b {Note:} By default the diffuse texture has a linear
    magnification filter, a linear mipmap linear minification filter,
    the wrap mode is repeat, the maximum anisotropy is set to 16.0
    and mipmapping is enabled.
 */
QAbstractTextureProvider *QNormalDiffuseSpecularMapMaterial::diffuse() const
{
    Q_D(const QNormalDiffuseSpecularMapMaterial);
    return d->m_diffuseParameter->value().value<QAbstractTextureProvider *>();
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::normal() const
    \return the current normal map texture.
    \b {Note:} By default the normal texture has linear minification
    and magnification filters, the wrap mode is repeat and maximum anisotropy
    is set to 16.0.
 */
QAbstractTextureProvider *QNormalDiffuseSpecularMapMaterial::normal() const
{
    Q_D(const QNormalDiffuseSpecularMapMaterial);
    return d->m_normalParameter->value().value<QAbstractTextureProvider *>();
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::specular() const
    \return the current specular map texture.
    \b {Note:} By default the specular texture has a linear
    magnification filter, a linear mipmap linear minification filter,
    the wrap mode is repeat, the maximum anisotropy is set to 16.0
    and mipmapping is enabled.
 */
QAbstractTextureProvider *QNormalDiffuseSpecularMapMaterial::specular() const
{
    Q_D(const QNormalDiffuseSpecularMapMaterial);
    return d->m_specularParameter->value().value<QAbstractTextureProvider *>();
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::shininess() const
    \return the current shininess as a float value.
 */
float QNormalDiffuseSpecularMapMaterial::shininess() const
{
    Q_D(const QNormalDiffuseSpecularMapMaterial);
    return d->m_shininessParameter->value().toFloat();
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::textureScale() const
    \return the current texture scale.
 */
float QNormalDiffuseSpecularMapMaterial::textureScale() const
{
    Q_D(const QNormalDiffuseSpecularMapMaterial);
    return d->m_textureScaleParameter->value().toFloat();
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::setAmbient(const QColor &ambient)
    Sets the current ambient color to \a ambient
 */
void QNormalDiffuseSpecularMapMaterial::setAmbient(const QColor &ambient)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    d->m_ambientParameter->setValue(ambient);
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::setDiffuse(QTexture *diffuse)
    Sets the current diffuse map to \a diffuse.
 */
void QNormalDiffuseSpecularMapMaterial::setDiffuse(QAbstractTextureProvider *diffuse)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    d->m_diffuseParameter->setValue(QVariant::fromValue(diffuse));
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::setNormal(QTexture *normal)
    Sets the current normal map to \a normal.
 */
void QNormalDiffuseSpecularMapMaterial::setNormal(QAbstractTextureProvider *normal)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    d->m_normalParameter->setValue(QVariant::fromValue(normal));
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::setSpecular(QTexture *specular)
    Sets the current specular map to \a specular.
 */
void QNormalDiffuseSpecularMapMaterial::setSpecular(QAbstractTextureProvider *specular)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    d->m_specularParameter->setValue(QVariant::fromValue(specular));
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::setShininess(float shininess)
    Sets the current shininess value to \a shininess.
 */
void QNormalDiffuseSpecularMapMaterial::setShininess(float shininess)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    d->m_shininessParameter->setValue(shininess);
}

/*!
    \fn Qt3D::QNormalDiffuseSpecularMapMaterial::setTextureScale(float textureScale)
    Sets the current texture scale to \a textureScale.
 */
void QNormalDiffuseSpecularMapMaterial::setTextureScale(float textureScale)
{
    Q_D(QNormalDiffuseSpecularMapMaterial);
    d->m_textureScaleParameter->setValue(textureScale);
}

} // Qt3D

QT_END_NAMESPACE
