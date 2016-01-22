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

#include "qpointlight.h"
#include "qpointlight_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*
  Expected Shader struct

  \code

  struct PointLight
  {
   vec3 position;
   vec4 color;
   float intensity;
  };

  uniform PointLight pointLights[10];

  \endcode
 */

QPointLightPrivate::QPointLightPrivate(QLight::Type type)
    : QLightPrivate(type)
    , m_attenuation(0.0f, 0.0f, 0.002f)
{
}

/*!
  \class Qt3DRender::QPointLight
  \inmodule Qt3DRender
  \since 5.5

 */

/*!
    \qmltype PointLight
    \instantiates Qt3DRender::QPointLight
    \inherits AbstractLight
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief For OpenGL ...
*/

/*!
  \fn Qt3DRender::QPointLight::QPointLight(Qt3DCore::QNode *parent)
  Constructs a new QPointLight with the specified \a parent.
 */
QPointLight::QPointLight(QNode *parent)
    : QLight(*new QPointLightPrivate, parent)
{
}

/*! \internal */
QPointLight::QPointLight(QPointLightPrivate &dd, QNode *parent)
    : QLight(dd, parent)
{
}

QVector3D QPointLight::attenuation() const
{
    Q_D(const QPointLight);
    return d->m_attenuation;
}

void QPointLight::setAttenuation(const QVector3D &value)
{
    Q_D(QPointLight);
    if (d->m_attenuation != value) {
        d->m_attenuation = value;
        emit attenuationChanged(value);
    }
}

float QPointLight::constantAttenuation() const
{
    Q_D(const QPointLight);
    return d->m_attenuation.x();
}

void QPointLight::setConstantAttenuation(float value)
{
    Q_D(QPointLight);
    if (d->m_attenuation.x() != value) {
        d->m_attenuation.setX(value);
        emit attenuationChanged(d->m_attenuation);
    }
}

float QPointLight::linearAttenuation() const
{
    Q_D(const QPointLight);
    return d->m_attenuation.y();
}

void QPointLight::setLinearAttenuation(float value)
{
    Q_D(QPointLight);
    if (d->m_attenuation.y() != value) {
        d->m_attenuation.setY(value);
        emit attenuationChanged(d->m_attenuation);
    }
}

float QPointLight::quadraticAttenuation() const
{
    Q_D(const QPointLight);
    return d->m_attenuation.z();
}

void QPointLight::setQuadraticAttenuation(float value)
{
    Q_D(QPointLight);
    if (d->m_attenuation.z() != value) {
        d->m_attenuation.setZ(value);
        emit attenuationChanged(d->m_attenuation);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
