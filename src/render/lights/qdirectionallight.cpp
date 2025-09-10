// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdirectionallight.h"
#include "qdirectionallight_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*
 *
 * Expected Shader struct
 *
 * \code
 *
 * struct DirectionalLight
 * {
 *  vec3 position;
 *  vec3 worldDirection;
 *  vec4 color;
 *  float intensity;
 * };
 *
 * uniform DirectionalLight directionalLights[10];
 *
 * \endcode
 */

QDirectionalLightPrivate::QDirectionalLightPrivate()
    : QAbstractLightPrivate(QAbstractLight::DirectionalLight)
{
    m_shaderData->setProperty("direction", QVector3D(0.0f, -1.0f, 0.0f));
}

/*!
  \class Qt3DRender::QDirectionalLight
  \inmodule Qt3DRender
  \since 5.7
  \brief Encapsulate a Directional Light object in a Qt 3D scene.

  A directional light is a light source that behaves similar to sunlight.
  The light from a directional light hits all objects from the same direction
  and with the same intensity, regardless of where they are in the scene.
 */

/*!
    \qmltype DirectionalLight
    \nativetype Qt3DRender::QDirectionalLight
    \inherits Light
    \inqmlmodule Qt3D.Render
    \since 5.7
    \brief Encapsulate a Directional Light object in a Qt 3D scene.

    A directional light is a light source that behaves similar to sunlight.
    The light from a directional light hits all objects from the same direction
    and with the same intensity, regardless of where they are in the scene.
*/

/*!
  \fn Qt3DRender::QDirectionalLight::QDirectionalLight(Qt3DCore::QNode *parent)
  Constructs a new QDirectionalLight with the specified \a parent.
 */
QDirectionalLight::QDirectionalLight(QNode *parent)
    : QAbstractLight(*new QDirectionalLightPrivate, parent)
{
}

/*! \internal */
QDirectionalLight::~QDirectionalLight()
{
}

/*! \internal */
QDirectionalLight::QDirectionalLight(QDirectionalLightPrivate &dd, QNode *parent)
    : QAbstractLight(dd, parent)
{
}

/*!
  \qmlproperty vector3d Qt3D.Render::DirectionalLight::worldDirection
    Specifies the world direction of the directional light.

    \note The exact meaning and use of this property is up to the
          material implementation.
*/

/*!
  \property Qt3DRender::QDirectionalLight::worldDirection
    Specifies the world direction of the directional light.

    \note The exact meaning and use of this property is up to the
          material implementation.
 */
void QDirectionalLight::setWorldDirection(const QVector3D &direction)
{
    Q_D(QDirectionalLight);
    if (worldDirection() != direction) {
        d->m_shaderData->setProperty("direction", direction);
        emit worldDirectionChanged(direction);
    }
}

QVector3D QDirectionalLight::worldDirection() const
{
    Q_D(const QDirectionalLight);
    return d->m_shaderData->property("direction").value<QVector3D>();
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qdirectionallight.cpp"
