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

#include "qabstractlight.h"
#include "qabstractlight_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D
{

/*!
 * \qmltype QAbstractLight
 * \instantiates QAbstractLight
 * \brief Encapsulate a QAbstractLight object in a Qt3D scene.
 * \since 5.3
 */


/*!
    \class Qt3D::QAbstractLightPrivate
    \internal
*/
QAbstractLightPrivate::QAbstractLightPrivate()
        : QShaderDataPrivate()
        , m_color(QColor(255, 255, 255))
        , m_intensity(1.0f)
{}

void QAbstractLight::copy(const QNode *ref)
{
    const QAbstractLight *light = static_cast<const QAbstractLight*>(ref);
    d_func()->m_color = light->d_func()->m_color;
    d_func()->m_intensity = light->d_func()->m_intensity;
    // This needs to be last otherwise, properties value won't be copied
    // as we use shader introspection in QShaderData::copy
    QShaderData::copy(ref);
}

/*!
    \class Qt3D::QAbstractLight
*/

/*!
 * Constructs a new QAbstractLight with the given \a parent.
 */
QAbstractLight::QAbstractLight(Qt3D::QNode *parent) :
    QShaderData(*new QAbstractLightPrivate, parent)
{
}

/*! \internal */
QAbstractLight::QAbstractLight(QAbstractLightPrivate &dd, QNode *parent)
    : QShaderData(dd, parent)
{
}


/*!
 *  \property Qt3D::QAbstractLight::color
 *
 * Holds the current QAbstractLight color.
 */
QColor QAbstractLight::color() const
{
    Q_D(const QAbstractLight);
    return d->m_color;
}

void QAbstractLight::setColor(const QColor &color)
{
    Q_D(QAbstractLight);
    if (d->m_color != color) {
        d->m_color = color;
        emit colorChanged();
    }
}

/*!
    \property Qt3D::QAbstractLight::intensity

    Holds the current QAbstractLight intensity.
*/
float QAbstractLight::intensity() const
{
    Q_D(const QAbstractLight);
    return d->m_intensity;
}

void QAbstractLight::setIntensity(float intensity)
{
    Q_D(QAbstractLight);
    if (d->m_intensity != intensity) {
        d->m_intensity = intensity;
        emit intensityChanged();
    }
}

/*!
    \property Qt3D::QAbstractLight::position

    Holds the current QAbstractLight position.
*/
void QAbstractLight::setPosition(const QVector3D &position)
{
    Q_D(QAbstractLight);
    if (d->m_position != position) {
        d->m_position = position;
        emit positionChanged();
    }
}

QVector3D QAbstractLight::position() const
{
    Q_D(const QAbstractLight);
    return d->m_position;
}

QShaderData::TransformType QAbstractLight::positionTransformed() const
{
    return QShaderData::ModelToEye;
}

} // Qt3D

QT_END_NAMESPACE
