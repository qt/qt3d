/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
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

QT_BEGIN_NAMESPACE

namespace Qt3D {

QPhongMaterialPrivate::QPhongMaterialPrivate(QPhongMaterial *qq)
    : QMaterialPrivate(qq)
    , m_phongEffect(new QEffect())
    , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
    , m_diffuseParameter(new QParameter(QStringLiteral("kd"), QColor::fromRgbF(0.7f, 0.7f, 0.7f, 1.0f)))
    , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
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

/*!
    \class QPhongMaterial
    \brief QPhongMaterial provides a default implementation of the phong lighting effect.
    \inmodule Qt3DRender
    \since 5.5

    The phong lighting effect is based on the combination of 3 lighting components ambient, diffuse and specular.
    The relative strengths of these components is controlled by means of their reflectivity coefficients which are modelled as RGB triplets:
    \li Ambient is the color that is emitted by an object without any other light source.
    \li Diffuse is the color that is emitted for rought surface reflections with the lights.
    \li Specular is the color emitted for shiny surface reflections with the lights.
    \li The shininess of a surface is controlled by a float property.

    This material uses an effect with a single render pass approach and performs per fragment lighting.
    Techniques are provided for OpenGL 2, OpenGL 3 or above as well as OpenGL ES 2.
*/

/*!
    \fn QPhongMaterial::QPhongMaterial
    Constructs a new QPhongMaterial instance with parent object \a parent.
 */
QPhongMaterial::QPhongMaterial(QNode *parent)
    : QMaterial(*new QPhongMaterialPrivate(this), parent)
{
    Q_D(QPhongMaterial);
    QObject::connect(d->m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
    QObject::connect(d->m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
    QObject::connect(d->m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
    QObject::connect(d->m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
    d->init();
}

QPhongMaterial::~QPhongMaterial()
{
}

/*!
    \fn QPhongMaterial::ambient
    \return the current QColor ambient value.
 */
QColor QPhongMaterial::ambient() const
{
    Q_D(const QPhongMaterial);
    return d->m_ambientParameter->value().value<QColor>();
}

/*!
    \fn QPhongMaterial::diffuse
    \return the current QColor diffuse value.
 */
QColor QPhongMaterial::diffuse() const
{
    Q_D(const QPhongMaterial);
    return d->m_diffuseParameter->value().value<QColor>();
}

/*!
    \fn QPhongMaterial::specular
    \return the current QColor specular value.
 */
QColor QPhongMaterial::specular() const
{
    Q_D(const QPhongMaterial);
    return d->m_specularParameter->value().value<QColor>();
}

/*!
    \fn QPhongMaterial::shininess
    \return the current shininess as a float value.
 */
float QPhongMaterial::shininess() const
{
    Q_D(const QPhongMaterial);
    return d->m_shininessParameter->value().toFloat();
}

/*!
    \fn QPhongMaterial::setAmbient
    Sets the current ambient color to \a ambient.
 */
void QPhongMaterial::setAmbient(const QColor &ambient)
{
    Q_D(QPhongMaterial);
    d->m_shininessParameter->setValue(ambient);
}

/*!
    \fn QPhongMaterial::setDiffuse
    Sets the current diffuse color to \a diffuse.
 */
void QPhongMaterial::setDiffuse(const QColor &diffuse)
{
    Q_D(QPhongMaterial);
    d->m_diffuseParameter->setValue(diffuse);
}

/*!
    \fn QPhongMaterial::setSpecular
    Sets the current specular color to \a specular.
 */
void QPhongMaterial::setSpecular(const QColor &specular)
{
    Q_D(QPhongMaterial);
    d->m_specularParameter->setValue(specular);
}

/*!
    \fn QPhongMaterial::setShininess
    Sets the current shininess value to \a shininess.
 */
void QPhongMaterial::setShininess(float shininess)
{
    Q_D(QPhongMaterial);
    d->m_shininessParameter->setValue(shininess);
}

//TO DO: Define how lights are properties are set in the shaders
//Ideally using a QShaderData
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

    static_cast<QMaterial *>(q_ptr)->setEffect(m_phongEffect);
}

} // Qt3D

QT_END_NAMESPACE
