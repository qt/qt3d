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

#include "qlight.h"
#include "qlight_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{

/*!
 * \qmltype Light
 * \inqmlmodule Qt3D.Render
 * \instantiates Qt3DRender::QLight
 * \brief Encapsulate a QLight object in a Qt 3D scene.
 * \since 5.6
 */


QLightPrivate::QLightPrivate(QLight::Type type)
    : m_type(type)
    , m_color(QColor(255, 255, 255))
    , m_intensity(1.0f)
{
}

void QLight::copy(const QNode *ref)
{
    const QLight *light = static_cast<const QLight*>(ref);
    d_func()->m_type = light->d_func()->m_type;
    d_func()->m_color = light->d_func()->m_color;
    d_func()->m_intensity = light->d_func()->m_intensity;
    QShaderData::copy(ref);
}

/*!
    \class Qt3DRender::QLight
    \inmodule Qt3DRender
*/

/*!
 * Constructs a new QLight with the given \a parent.
 */
QLight::QLight(Qt3DCore::QNode *parent) :
    QShaderData(*new QLightPrivate(PointLight), parent)
{
}

/*! \internal */
QLight::QLight(QLightPrivate &dd, QNode *parent)
    : QShaderData(dd, parent)
{
}

QLight::~QLight()
{
    cleanup();
}

QLight::Type QLight::type() const
{
    Q_D(const QLight);
    return d->m_type;
}

void QLight::setType(Type type)
{
    Q_D(QLight);
    if (d->m_type != type) {
        d->m_type = type;
        emit typeChanged(type);
    }
}

/*!
 *  \property Qt3DRender::QLight::color
 *
 * Holds the current QLight color.
 */
QColor QLight::color() const
{
    Q_D(const QLight);
    return d->m_color;
}

void QLight::setColor(const QColor &color)
{
    Q_D(QLight);
    if (d->m_color != color) {
        d->m_color = color;
        emit colorChanged(color);
    }
}

/*!
    \property Qt3DRender::QLight::intensity

    Holds the current QLight intensity.
*/
float QLight::intensity() const
{
    Q_D(const QLight);
    return d->m_intensity;
}

void QLight::setIntensity(float intensity)
{
    Q_D(QLight);
    if (d->m_intensity != intensity) {
        d->m_intensity = intensity;
        emit intensityChanged(intensity);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
