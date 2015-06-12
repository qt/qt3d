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

namespace Qt3D {

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

/*!
    \class Qt3D::QPointLightPrivate
    \internal
*/
QPointLightPrivate::QPointLightPrivate()
    : QAbstractLightPrivate()
{
}

/*!
  \class QPointLight
  \inmodule Qt3DRenderer
  \since 5.5

 */

/*!
    \qmltype PointLight
    \instantiates Qt3D::QPointLight
    \inherits AbstractLight
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief For OpenGL ...
*/

/*!
  \fn Qt3D::QPointLight::QPointLight(QNode *parent)
  Constructs a new QPointLight with the specified \a parent.
 */
QPointLight::QPointLight(QNode *parent)
    : QAbstractLight(*new QPointLightPrivate, parent)
{
}

/*! \internal */
QPointLight::QPointLight(QPointLightPrivate &dd, QNode *parent)
    : QAbstractLight(dd, parent)
{
}

} // Qt3D

QT_END_NAMESPACE
