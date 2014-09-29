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


QAbstractLightPrivate::QAbstractLightPrivate(QAbstractLight *qq)
        : QComponentPrivate(qq)
        , m_color(QColor(255, 255, 255))
        , m_intensity(1.0f)
    {}

void QAbstractLightPrivate::copy(const QNodePrivate *ref)
{
    QComponentPrivate::copy(ref);
    const QAbstractLightPrivate *light = static_cast<const QAbstractLightPrivate *>(ref);
    m_color = light->m_color;
    m_intensity = light->m_intensity;
}

/*!
 * Constructs a new QAbstractLight with the given \a parent.
 */
QAbstractLight::QAbstractLight(Qt3D::QNode *parent) :
    QComponent(*new QAbstractLightPrivate(this), parent)
{
}


QAbstractLight::QAbstractLight(QAbstractLightPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}


/*!
 * Returns the current QAbstractLight color.
 */
QColor QAbstractLight::color() const
{
    Q_D(const QAbstractLight);
    return d->m_color;
}

/*!
 * Sets the current QAbstractLight \a color;
 *
 * \sa void QAbstractLight::colorChanged();
 */
void QAbstractLight::setColor(const QColor &color)
{
    Q_D(QAbstractLight);
    if (d->m_color != color)
    {
        d->m_color = color;
        emit colorChanged();
    }
}

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

} // Qt3D

QT_END_NAMESPACE
