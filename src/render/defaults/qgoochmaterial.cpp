/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qgoochmaterial.h"
#include "qgoochmaterial_p.h"

#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qopenglfilter.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qtechnique.h>

#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
   \class Qt3D::QGoochMaterialPrivate
   \internal
*/
QGoochMaterialPrivate::QGoochMaterialPrivate()
    : QMaterialPrivate()
    , m_effect(new QEffect)
    , m_diffuseParameter(new QParameter(QStringLiteral("kd"), QColor::fromRgbF(0.0f, 0.0f, 0.0f)))
    , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.0f, 0.0f, 0.0f)))
    , m_coolParameter(new QParameter(QStringLiteral("kblue"), QColor::fromRgbF(0.0f, 0.0f, 0.4f)))
    , m_warmParameter(new QParameter(QStringLiteral("kyellow"), QColor::fromRgbF(0.4f, 0.4f, 0.0f)))
    , m_alphaParameter(new QParameter(QStringLiteral("alpha"), 0.25f))
    , m_betaParameter(new QParameter(QStringLiteral("alpha"), 0.5f))
    , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 100.0f))
    , m_gl3Technique(new QTechnique)
    , m_gl2Technique(new QTechnique)
    , m_es2Technique(new QTechnique)
    , m_gl3RenderPass(new QRenderPass)
    , m_gl2RenderPass(new QRenderPass)
    , m_es2RenderPass(new QRenderPass)
    , m_gl3Shader(new QShaderProgram)
    , m_gl2ES2Shader(new QShaderProgram)
{
}

void QGoochMaterialPrivate::init()
{
    Q_Q(QGoochMaterial);
    QObject::connect(m_diffuseParameter, SIGNAL(valueChanged()), q, SIGNAL(diffuseChanged()));
    QObject::connect(m_specularParameter, SIGNAL(valueChanged()), q, SIGNAL(specularChanged()));
    QObject::connect(m_coolParameter, SIGNAL(valueChanged()), q, SIGNAL(coolChanged()));
    QObject::connect(m_warmParameter, SIGNAL(valueChanged()), q, SIGNAL(warmChanged()));
    QObject::connect(m_alphaParameter, SIGNAL(valueChanged()), q, SIGNAL(alphaChanged()));
    QObject::connect(m_betaParameter, SIGNAL(valueChanged()), q, SIGNAL(betaChanged()));
    QObject::connect(m_shininessParameter, SIGNAL(valueChanged()), q, SIGNAL(shininessChanged()));

    m_gl3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/gooch.vert"))));
    m_gl3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/gooch.frag"))));
    m_gl2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/gooch.vert"))));
    m_gl2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/gooch.frag"))));

    m_gl3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_gl3Technique->openGLFilter()->setMajorVersion(3);
    m_gl3Technique->openGLFilter()->setMinorVersion(1);
    m_gl3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_gl2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_gl2Technique->openGLFilter()->setMajorVersion(2);
    m_gl2Technique->openGLFilter()->setMinorVersion(0);
    m_gl2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_es2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_es2Technique->openGLFilter()->setMajorVersion(2);
    m_es2Technique->openGLFilter()->setMinorVersion(0);
    m_es2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_gl3RenderPass->setShaderProgram(m_gl3Shader);
    m_gl2RenderPass->setShaderProgram(m_gl2ES2Shader);
    m_es2RenderPass->setShaderProgram(m_gl2ES2Shader);

    m_gl3Technique->addPass(m_gl3RenderPass);
    m_gl2Technique->addPass(m_gl2RenderPass);
    m_es2Technique->addPass(m_es2RenderPass);

    m_effect->addTechnique(m_gl3Technique);
    m_effect->addTechnique(m_gl2Technique);
    m_effect->addTechnique(m_es2Technique);

    m_effect->addParameter(m_diffuseParameter);
    m_effect->addParameter(m_specularParameter);
    m_effect->addParameter(m_coolParameter);
    m_effect->addParameter(m_warmParameter);
    m_effect->addParameter(m_alphaParameter);
    m_effect->addParameter(m_betaParameter);
    m_effect->addParameter(m_shininessParameter);

    q->setEffect(m_effect);
}

/*!
    \class Qt3D::QGoochMaterial
    \brief The QGoochMaterial provides a material that implements the Gooch
    shading model, popular in CAD and CAM applications.
    \inmodule Qt3DRenderer
    \since 5.5

    The Gooch lighting model uses both color and brightness to help show the
    curvature of 3D surfaces. This is often better than models such as Phong
    that rely purely upon changes in brightness. In situations such as in CAD
    and CAM applications where photorealism is not a goal, the Gooch shading
    model in conjunction with some kind of silhouette edge inking is a popular
    solution.

    The Gooch lighting model is explained fully in the \l{original Gooch
    paper}. The Gooch model mixes a diffuse objetc color with a user-provided
    cool color and warm color to produce the end points of a color ramp that is
    used to shade the object based upon the cosine of the angle between the
    vector from the fragment to the light source and the fragment's normal
    vector. Optionally, a specular highlight can be added on top. The relative
    contributions to the cool and warm colors by the diffuse color are
    controlled by the alpha and beta properties respecitvely.

    This material uses an effect with a single render pass approach and
    performs per fragment lighting. Techniques are provided for OpenGL 2,
    OpenGL 3 or above as well as OpenGL ES 2.
*/

/*!
    Constructs a new Qt3D::QGoochMaterial instance with parent object \a parent.
*/
QGoochMaterial::QGoochMaterial(QNode *parent)
    : QMaterial(*new QGoochMaterialPrivate, parent)
{
    Q_D(QGoochMaterial);
    d->init();
}

QGoochMaterial::QGoochMaterial(QGoochMaterialPrivate &dd, QNode *parent)
    : QMaterial(dd, parent)
{
    Q_D(QGoochMaterial);
    d->init();
}

/*!
    \property Qt3D::QGoochMaterial::diffuse

    Holds the current diffuse color.
*/
QColor QGoochMaterial::diffuse() const
{
    Q_D(const QGoochMaterial);
    return d->m_diffuseParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QGoochMaterial::specular

    Holds the current specular color.
*/
QColor QGoochMaterial::specular() const
{
    Q_D(const QGoochMaterial);
    return d->m_specularParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QGoochMaterial::cool

    Holds the current cool color.
*/
QColor QGoochMaterial::cool() const
{
    Q_D(const QGoochMaterial);
    return d->m_coolParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QGoochMaterial::warm

    Holds the current warm color.
*/
QColor QGoochMaterial::warm() const
{
    Q_D(const QGoochMaterial);
    return d->m_warmParameter->value().value<QColor>();
}

/*!
    \property Qt3D::QGoochMaterial::alpha

    Holds the current alpha value. The start point of the color ramp
    used by the Gooch shader is calculated as {c = cool + alpha * diffuse}.
*/
float QGoochMaterial::alpha() const
{
    Q_D(const QGoochMaterial);
    return d->m_alphaParameter->value().toFloat();
}

/*!
    \property Qt3D::QGoochMaterial::beta

    Holds the current beta value. The start point of the color ramp
    used by the Gooch shader is calculated as {c = warm + beta * diffuse}.
*/
float QGoochMaterial::beta() const
{
    Q_D(const QGoochMaterial);
    return d->m_betaParameter->value().toFloat();
}

/*!
    \property Qt3D::QGoochMaterial::shininess

    Holds the current shininess value. Higher values of shininess result in
    a smaller and brighter highlight.
*/
float QGoochMaterial::shininess() const
{
    Q_D(const QGoochMaterial);
    return d->m_shininessParameter->value().toFloat();
}

void QGoochMaterial::setDiffuse(const QColor &diffuse)
{
    Q_D(QGoochMaterial);
    return d->m_diffuseParameter->setValue(diffuse);
}

void QGoochMaterial::setSpecular(const QColor &specular)
{
    Q_D(QGoochMaterial);
    return d->m_specularParameter->setValue(specular);
}

void QGoochMaterial::setCool(const QColor &cool)
{
    Q_D(QGoochMaterial);
    return d->m_coolParameter->setValue(cool);
}

void QGoochMaterial::setWarm(const QColor &warm)
{
    Q_D(QGoochMaterial);
    return d->m_warmParameter->setValue(warm);
}

void QGoochMaterial::setAlpha(float alpha)
{
    Q_D(QGoochMaterial);
    return d->m_alphaParameter->setValue(alpha);
}

void QGoochMaterial::setBeta(float beta)
{
    Q_D(QGoochMaterial);
    return d->m_betaParameter->setValue(beta);
}

void QGoochMaterial::setShininess(float shininess)
{
    Q_D(QGoochMaterial);
    return d->m_shininessParameter->setValue(shininess);
}

}

QT_END_NAMESPACE
