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
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
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
    \class Qt3D::QPhongMaterialPrivate
    \internal
*/
QPhongMaterialPrivate::QPhongMaterialPrivate()
    : QMaterialPrivate()
    , m_phongEffect(new QEffect())
    , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
    , m_diffuseParameter(new QParameter(QStringLiteral("kd"), QColor::fromRgbF(0.7f, 0.7f, 0.7f, 1.0f)))
    , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
    , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
    , m_lightPositionParameter(new QParameter(QStringLiteral("lightPosition"), QVector4D(1.0f, 1.0f, 0.0f, 1.0f)))
    , m_lightIntensityParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
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

/*!
    \class Qt3D::QPhongMaterial
    \brief The QPhongMaterial class provides a default implementation of the phong lighting effect.
    \inmodule Qt3DRenderer
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
    \fn Qt3D::QPhongMaterial::QPhongMaterial(QNode *parent)

    Constructs a new QPhongMaterial instance with parent object \a parent.
*/
QPhongMaterial::QPhongMaterial(QNode *parent)
    : QMaterial(*new QPhongMaterialPrivate, parent)
{
    Q_D(QPhongMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    d->init();
}

/*!
   \fn Qt3D::QPhongMaterial::~QPhongMaterial()

   Destroys the QPhongMaterial.
*/
QPhongMaterial::~QPhongMaterial()
{
}

/*!
    \property Qt3D::QPhongMaterial::ambient

    Holds the ambient color.
*/
QColor QPhongMaterial::ambient() const
{
    Q_D(const QPhongMaterial);
    return d->m_ambientParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QPhongMaterial::diffuse

    Holds the diffuse color.
*/
QColor QPhongMaterial::diffuse() const
{
    Q_D(const QPhongMaterial);
    return d->m_diffuseParameter->value().value<QColor>();
}

/*!
    \property QColor Qt3D::QPhongMaterial::specular

    Holds the specular color.
*/
QColor QPhongMaterial::specular() const
{
    Q_D(const QPhongMaterial);
    return d->m_specularParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QPhongMaterial::shininess

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

// TODO: Define how lights are properties are set in the shaders. Ideally using a QShaderData
void QPhongMaterialPrivate::init()
{
    m_phongGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/phong.vert"))));
    m_phongGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/phong.frag"))));
    m_phongGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/phong.vert"))));
    m_phongGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/phong.frag"))));

    m_phongGL3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_phongGL3Technique->openGLFilter()->setMajorVersion(3);
    m_phongGL3Technique->openGLFilter()->setMinorVersion(1);
    m_phongGL3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_phongGL2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_phongGL2Technique->openGLFilter()->setMajorVersion(2);
    m_phongGL2Technique->openGLFilter()->setMinorVersion(0);
    m_phongGL2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_phongES2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_phongES2Technique->openGLFilter()->setMajorVersion(2);
    m_phongES2Technique->openGLFilter()->setMinorVersion(0);
    m_phongES2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

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
    m_phongEffect->addParameter(m_lightPositionParameter);
    m_phongEffect->addParameter(m_lightIntensityParameter);

    q_func()->setEffect(m_phongEffect);
}

} // Qt3D

QT_END_NAMESPACE
