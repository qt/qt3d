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

#define _USE_MATH_DEFINES // For MSVC
#include "rotatetransform.h"

#include <cmath>

QT_BEGIN_NAMESPACE

namespace Qt3D {

RotateTransform::RotateTransform(Node *parent)
    : AbstractTransform(parent)
{
}

float RotateTransform::angleDeg() const
{
    return m_angleDeg;
}

float Qt3D::RotateTransform::angleRad() const
{
    return (m_angleDeg / 180.0) * M_PI;
}

QVector3D RotateTransform::axis() const
{
    return m_axis;
}

QMatrix4x4 RotateTransform::matrix() const
{
    QMatrix4x4 m;
    m.rotate(m_angleDeg, m_axis);
    return m;
}

void RotateTransform::setAngleDeg(float arg)
{
    if (m_angleDeg != arg) {
        m_angleDeg = arg;
        emit angleChanged();
        emit transformUpdated();
    }
}

void RotateTransform::setAngleRad(float arg)
{
    float angleDeg = (arg * M_PI) / 180.0f;
    setAngleDeg(angleDeg);
}

void RotateTransform::setAxis(const QVector3D& arg)
{
    if (m_axis != arg) {
        m_axis = arg;
        emit axisChanged();
        emit transformUpdated();
    }
}

} // namespace Qt3D

QT_END_NAMESPACE
