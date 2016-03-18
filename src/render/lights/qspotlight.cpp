/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qspotlight.h"
#include "qspotlight_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {


/*
  Expected Shader struct

  \code

  struct SpotLight
  {
   vec3 position;
   vec3 localDirection;
   vec4 color;
   float intensity;
   float cutOffAngle;
  };

  uniform SpotLight spotLights[10];

  \endcode
 */

QSpotLightPrivate::QSpotLightPrivate()
    : QLightPrivate(QLight::SpotLight)
    , m_attenuation(0.0f, 0.0f, 0.002f)
    , m_localDirection(0.0f, -1.0f, 0.0f)
    , m_cutOffAngle(45.0f)
{
}

/*!
  \class Qt3DRender::QSpotLight
  \inmodule Qt3DRender
  \since 5.5

 */

/*!
    \qmltype SpotLight
    \instantiates Qt3DRender::QSpotLight
    \inherits AbstractLight
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief For OpenGL ...
*/

/*! \fn void Qt3DRender::QSpotLight::copy(const Qt3DCore::QNode *ref)
  Copies the \a ref instance into this one.
 */

void QSpotLight::copy(const QNode *ref)
{
    const QSpotLight *light = static_cast<const QSpotLight*>(ref);
    d_func()->m_localDirection = light->d_func()->m_localDirection;
    d_func()->m_cutOffAngle = light->d_func()->m_cutOffAngle;
    QLight::copy(ref);
}


/*!
  \fn Qt3DRender::QSpotLight::QSpotLight(Qt3DCore::QNode *parent)
  Constructs a new QSpotLight with the specified \a parent.
 */
QSpotLight::QSpotLight(QNode *parent)
    : QLight(*new QSpotLightPrivate, parent)
{
}

/*! \internal */
QSpotLight::QSpotLight(QSpotLightPrivate &dd, QNode *parent)
    : QLight(dd, parent)
{
}

/*!
  \qmlproperty vector3d Qt3D.Render::SpotLight::localDirection

*/

/*!
  \property Qt3DRender::QSpotLight::localDirection

 */

QVector3D QSpotLight::attenuation() const
{
    Q_D(const QSpotLight);
    return d->m_attenuation;
}

float QSpotLight::constantAttenuation() const
{
    Q_D(const QSpotLight);
    return d->m_attenuation.x();
}

float QSpotLight::linearAttenuation() const
{
    Q_D(const QSpotLight);
    return d->m_attenuation.y();
}

float QSpotLight::quadraticAttenuation() const
{
    Q_D(const QSpotLight);
    return d->m_attenuation.z();
}

    QVector3D QSpotLight::localDirection() const
{
    Q_D(const QSpotLight);
    return d->m_localDirection;
}


/*!
  \qmlproperty float Qt3D.Render::SpotLight::cutOffAngle

*/

/*!
  \property Qt3DRender::QSpotLight::cutOffAngle

 */
float QSpotLight::cutOffAngle() const
{
    Q_D(const QSpotLight);
    return d->m_cutOffAngle;
}

void QSpotLight::setAttenuation(const QVector3D &value)
{
    Q_D(QSpotLight);
    if (d->m_attenuation != value) {
        d->m_attenuation = value;
        emit attenuationChanged(value);
    }
}

void QSpotLight::setConstantAttenuation(float value)
{
    Q_D(QSpotLight);
    if (d->m_attenuation.x() != value) {
        d->m_attenuation.setX(value);
        emit attenuationChanged(d->m_attenuation);
    }
}

void QSpotLight::setLinearAttenuation(float value)
{
    Q_D(QSpotLight);
    if (d->m_attenuation.y() != value) {
        d->m_attenuation.setY(value);
        emit attenuationChanged(d->m_attenuation);
    }
}

void QSpotLight::setQuadraticAttenuation(float value)
{
    Q_D(QSpotLight);
    if (d->m_attenuation.z() != value) {
        d->m_attenuation.setZ(value);
        emit attenuationChanged(d->m_attenuation);
    }
}

void QSpotLight::setLocalDirection(const QVector3D &localDirection)
{
    Q_D(QSpotLight);
    if (localDirection != d->m_localDirection) {
        d->m_localDirection = localDirection;
        emit localDirectionChanged(localDirection);
    }
}

void QSpotLight::setCutOffAngle(float cutOffAngle)
{
    Q_D(QSpotLight);
    if (d->m_cutOffAngle != cutOffAngle) {
        d->m_cutOffAngle = cutOffAngle;
        emit cutOffAngleChanged(cutOffAngle);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
