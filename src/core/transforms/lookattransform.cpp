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

#include "lookattransform.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

LookAtTransform::LookAtTransform(QNode *parent)
    : AbstractTransform(parent)
    , m_matrixDirty(true)
{
}

QMatrix4x4 LookAtTransform::matrix() const
{
    if (m_matrixDirty) {
        m_matrix.setToIdentity();
        m_matrix.lookAt(m_position, m_viewCenter, m_upVector);
        m_matrixDirty = false;
    }
    return m_matrix;
}

QVector3D LookAtTransform::position() const
{
    return m_position;
}

void LookAtTransform::setPosition(const QVector3D &position)
{
    if (m_position != position) {
        m_position = position;
        m_viewVector = m_viewCenter - position;
        m_matrixDirty = true;
        emit positionChanged();
        emit viewVectorChanged();
        emit transformUpdated();
    }
}

void LookAtTransform::setUpVector(const QVector3D &upVector)
{
    if (m_upVector != upVector) {
        m_upVector = upVector;
        m_matrixDirty = true;
        emit upVectorChanged();
        emit transformUpdated();
    }
}

QVector3D LookAtTransform::upVector() const
{
    return m_upVector;
}

void LookAtTransform::setViewCenter(const QVector3D &viewCenter)
{
    if (m_viewCenter != viewCenter) {
        m_viewCenter = viewCenter;
        m_viewVector = viewCenter - m_position;
        m_matrixDirty = true;
        emit viewCenterChanged();
        emit viewVectorChanged();
        emit transformUpdated();
    }
}

QVector3D LookAtTransform::viewCenter() const
{
    return m_viewCenter;
}

void LookAtTransform::setViewVector(const QVector3D &viewVector)
{
    m_viewVector = viewVector;
}

QVector3D LookAtTransform::viewVector() const
{
    return m_viewVector;
}

} // namespace Qt3D

QT_END_NAMESPACE
