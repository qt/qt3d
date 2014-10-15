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

#include "qspotlight.h"
#include "qspotlight_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {


/*!
 *
 * Expected Shader struct
 *
 * \code
 *
 * struct SpotLight
 * {
 *  vec3 position;
 *  vec3 direction;
 *  vec4 color;
 *  float intensity;
 *  float cutOffAngle;
 * };
 *
 * uniform SpotLight spotLights[10];
 *
 * \endcode
 */

QSpotLightPrivate::QSpotLightPrivate(QSpotLight *qq)
    :QAbstractLightPrivate(qq)
    , m_cutOffAngle(45.0f)
{
}

void QSpotLight::copy(const QNode *ref)
{
    QAbstractLight::copy(ref);
    const QSpotLight *light = static_cast<const QSpotLight*>(ref);
    d_func()->m_direction = light->d_func()->m_direction;
    d_func()->m_cutOffAngle = light->d_func()->m_cutOffAngle;
}

QSpotLight::QSpotLight(QNode *parent)
    : QAbstractLight(*new QSpotLightPrivate(this), parent)
{
}

QSpotLight::QSpotLight(QSpotLightPrivate &dd, QNode *parent)
    : QAbstractLight(dd, parent)
{
}

QVector3D QSpotLight::direction() const
{
    Q_D(const QSpotLight);
    return d->m_direction;
}

float QSpotLight::cutOffAngle() const
{
    Q_D(const QSpotLight);
    return d->m_cutOffAngle;
}

void QSpotLight::setDirection(const QVector3D &direction)
{
    Q_D(QSpotLight);
    if (direction != d->m_direction) {
        d->m_direction = direction;
        emit directionChanged();
    }
}

void QSpotLight::setCutOffAngle(float cutOffAngle)
{
    Q_D(QSpotLight);
    if (d->m_cutOffAngle != cutOffAngle) {
        d->m_cutOffAngle = cutOffAngle;
        emit cutOffAngleChanged();
    }
}

QHash<QString, QVariant> QSpotLight::lightProperties() const
{
    Q_D(const QSpotLight);
    QHash<QString, QVariant> props;
    props[QStringLiteral("direction")] = d->m_direction;
    props[QStringLiteral("cutOffAngle")] = d->m_cutOffAngle;
    return props;
}

} // Qt3D

QT_END_NAMESPACE
