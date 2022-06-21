// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qabstractlight.h"
#include "qabstractlight_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{

/*!
 * \qmltype Light
 * \inqmlmodule Qt3D.Render
 * \instantiates Qt3DRender::QAbstractLight
 * \brief Encapsulate a QAbstractLight object in a Qt 3D scene.
 * \since 5.6
 */

QAbstractLightPrivate::QAbstractLightPrivate(QAbstractLight::Type type)
    : m_type(type)
    , m_shaderData(new QShaderData)
{
    m_shaderData->setProperty("type", type);
    m_shaderData->setProperty("color", QColor(Qt::white));
    m_shaderData->setProperty("intensity", 0.5f);
}

QAbstractLightPrivate::~QAbstractLightPrivate()
{
}

/*!
    \qmlproperty enumeration Qt3D.Render::Light::type
    \readonly

    Holds the particular type of light.

    \value Light.PointLight
           A point light
    \value Light.DirectionalLight
           A directional light
    \value Light.SpotLight
           a spot light
*/
/*!
    \property Qt3DRender::QAbstractLight::type

    The type of light.
*/
/*!
    \enum Qt3DRender::QAbstractLight::Type

    Identifies the particular type of light.

    \value PointLight
    \value DirectionalLight
    \value SpotLight
*/

/*!
    \class Qt3DRender::QAbstractLight
    \inmodule Qt3DRender
    \brief Encapsulate a QAbstractLight object in a Qt 3D scene.
    \since 5.6
*/

/*! \internal */
QAbstractLight::QAbstractLight(QAbstractLightPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
    Q_D(QAbstractLight);
    d->m_shaderData->setParent(this);
}

QAbstractLight::~QAbstractLight()
{
}

/*!
    Holds the current QAbstractLight type.
*/
QAbstractLight::Type QAbstractLight::type() const
{
    Q_D(const QAbstractLight);
    return d->m_type;
}

/*!
 *  \qmlproperty QColor Qt3D.Render.Light::color
 *
 *  Holds the current Light color.
 */
/*!
 *  \property Qt3DRender::QAbstractLight::color
 *
 * Holds the current QAbstractLight color.
 */
QColor QAbstractLight::color() const
{
    Q_D(const QAbstractLight);
    return d->m_shaderData->property("color").value<QColor>();
}

void QAbstractLight::setColor(const QColor &c)
{
    Q_D(QAbstractLight);
    if (color() != c) {
        d->m_shaderData->setProperty("color", c);
        emit colorChanged(c);
    }
}

/*!
    \qmlproperty float Qt3D.Render.Light::intensity

    Holds the current Light intensity.
*/
/*!
    \property Qt3DRender::QAbstractLight::intensity

    Holds the current QAbstractLight intensity.
*/
float QAbstractLight::intensity() const
{
    Q_D(const QAbstractLight);
    return d->m_shaderData->property("intensity").toFloat();
}

void QAbstractLight::setIntensity(float value)
{
    Q_D(QAbstractLight);
    if (intensity() != value) {
        d->m_shaderData->setProperty("intensity", value);
        emit intensityChanged(value);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qabstractlight.cpp"
