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

#include "qphongmaterial.h"
#include "qphongmaterial_p.h"
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QPhongMaterialPrivate::QPhongMaterialPrivate()
    : QMaterialPrivate()
    , m_phongEffect(new QEffect())
    , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
    , m_diffuseParameter(new QParameter(QStringLiteral("kd"), QColor::fromRgbF(0.7f, 0.7f, 0.7f, 1.0f)))
    , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.01f, 0.01f, 0.01f, 1.0f)))
    , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
    , m_phongGL3Technique(new QTechnique())
    , m_phongGL2Technique(new QTechnique())
    , m_phongES2Technique(new QTechnique())
    , m_phongGL3RenderPass(new QRenderPass())
    , m_phongGL2RenderPass(new QRenderPass())
    , m_phongES2RenderPass(new QRenderPass())
    , m_phongGL3Shader(new QShaderProgram())
    , m_phongGL2ES2Shader(new QShaderProgram())
{
}

// TODO: Define how lights are properties are set in the shaders. Ideally using a QShaderData
void QPhongMaterialPrivate::init()
{
    connect(m_ambientParameter, &Qt3DRender::QParameter::valueChanged,
            this, &QPhongMaterialPrivate::handleAmbientChanged);
    connect(m_diffuseParameter, &Qt3DRender::QParameter::valueChanged,
            this, &QPhongMaterialPrivate::handleDiffuseChanged);
    connect(m_specularParameter, &Qt3DRender::QParameter::valueChanged,
            this, &QPhongMaterialPrivate::handleSpecularChanged);
    connect(m_shininessParameter, &Qt3DRender::QParameter::valueChanged,
            this, &QPhongMaterialPrivate::handleShininessChanged);

    m_phongGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/phong.vert"))));
    m_phongGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/phong.frag"))));
    m_phongGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/phong.vert"))));
    m_phongGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/phong.frag"))));

    m_phongGL3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    m_phongGL3Technique->graphicsApiFilter()->setMajorVersion(3);
    m_phongGL3Technique->graphicsApiFilter()->setMinorVersion(1);
    m_phongGL3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

    m_phongGL2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    m_phongGL2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_phongGL2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_phongGL2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

    m_phongES2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGLES);
    m_phongES2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_phongES2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_phongES2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

    m_phongGL3RenderPass->setShaderProgram(m_phongGL3Shader);
    m_phongGL2RenderPass->setShaderProgram(m_phongGL2ES2Shader);
    m_phongES2RenderPass->setShaderProgram(m_phongGL2ES2Shader);

    m_phongGL3Technique->addPass(m_phongGL3RenderPass);
    m_phongGL2Technique->addPass(m_phongGL2RenderPass);
    m_phongES2Technique->addPass(m_phongES2RenderPass);

    m_phongEffect->addTechnique(m_phongGL3Technique);
    m_phongEffect->addTechnique(m_phongGL2Technique);
    m_phongEffect->addTechnique(m_phongES2Technique);

    m_phongEffect->addParameter(m_ambientParameter);
    m_phongEffect->addParameter(m_diffuseParameter);
    m_phongEffect->addParameter(m_specularParameter);
    m_phongEffect->addParameter(m_shininessParameter);

    q_func()->setEffect(m_phongEffect);
}

void QPhongMaterialPrivate::handleAmbientChanged(const QVariant &var)
{
    Q_Q(QPhongMaterial);
    emit q->ambientChanged(var.value<QColor>());
}

void QPhongMaterialPrivate::handleDiffuseChanged(const QVariant &var)
{
    Q_Q(QPhongMaterial);
    emit q->diffuseChanged(var.value<QColor>());
}

void QPhongMaterialPrivate::handleSpecularChanged(const QVariant &var)
{
    Q_Q(QPhongMaterial);
    emit q->specularChanged(var.value<QColor>());
}

void QPhongMaterialPrivate::handleShininessChanged(const QVariant &var)
{
    Q_Q(QPhongMaterial);
    emit q->shininessChanged(var.toFloat());
}

/*!
    \class Qt3DRender::QPhongMaterial
    \brief The QPhongMaterial class provides a default implementation of the phong lighting effect.
    \inmodule Qt3DRender
    \since 5.5

    The phong lighting effect is based on the combination of 3 lighting components ambient, diffuse and specular.
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
    \fn Qt3DRender::QPhongMaterial::QPhongMaterial(Qt3DCore::QNode *parent)

    Constructs a new QPhongMaterial instance with parent object \a parent.
*/
QPhongMaterial::QPhongMaterial(QNode *parent)
    : QMaterial(*new QPhongMaterialPrivate, parent)
{
    Q_D(QPhongMaterial);
    d->init();
}

/*!
   \fn Qt3DRender::QPhongMaterial::~QPhongMaterial()

   Destroys the QPhongMaterial.
*/
QPhongMaterial::~QPhongMaterial()
{
}

/*!
    \property Qt3DRender::QPhongMaterial::ambient

    Holds the ambient color.
*/
QColor QPhongMaterial::ambient() const
{
    Q_D(const QPhongMaterial);
    return d->m_ambientParameter->value().value<QColor>();
}

/*!
    \property Qt3DRender::QPhongMaterial::diffuse

    Holds the diffuse color.
*/
QColor QPhongMaterial::diffuse() const
{
    Q_D(const QPhongMaterial);
    return d->m_diffuseParameter->value().value<QColor>();
}

/*!
    \property Qt3DRender::QPhongMaterial::specular

    Holds the specular color.
*/
QColor QPhongMaterial::specular() const
{
    Q_D(const QPhongMaterial);
    return d->m_specularParameter->value().value<QColor>();
}

/*!
    \property Qt3DRender::QPhongMaterial::shininess

    Holds the shininess exponent.
*/
float QPhongMaterial::shininess() const
{
    Q_D(const QPhongMaterial);
    return d->m_shininessParameter->value().toFloat();
}

void QPhongMaterial::setAmbient(const QColor &ambient)
{
    Q_D(QPhongMaterial);
    d->m_ambientParameter->setValue(ambient);
}

void QPhongMaterial::setDiffuse(const QColor &diffuse)
{
    Q_D(QPhongMaterial);
    d->m_diffuseParameter->setValue(diffuse);
}

void QPhongMaterial::setSpecular(const QColor &specular)
{
    Q_D(QPhongMaterial);
    d->m_specularParameter->setValue(specular);
}

void QPhongMaterial::setShininess(float shininess)
{
    Q_D(QPhongMaterial);
    d->m_shininessParameter->setValue(shininess);
}

} // namespace Qt3DRender

QT_END_NAMESPACE
