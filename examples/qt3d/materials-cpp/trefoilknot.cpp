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

#include "trefoilknot.h"

TrefoilKnot::TrefoilKnot(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_mesh(new Qt3DRender::QMesh())
    , m_transform(new Qt3DCore::QTransform())
    , m_position()
    , m_scale(1.0f)
{
    m_mesh->setSource(QUrl("qrc:/assets/obj/trefoil.obj"));
    addComponent(m_mesh);
    addComponent(m_transform);
}

TrefoilKnot::~TrefoilKnot()
{
}

void TrefoilKnot::updateTransform()
{
    QMatrix4x4 m;
    m.translate(m_position);
    m.rotate(m_phi, QVector3D(1.0f, 0.0f, 0.0f));
    m.rotate(m_phi, QVector3D(0.0f, 1.0f, 0.0f));
    m.scale(m_scale);
    m_transform->setMatrix(m);
}

float TrefoilKnot::theta() const
{
    return m_theta;
}

float TrefoilKnot::phi() const
{
    return m_phi;
}

QVector3D TrefoilKnot::position() const
{
    return m_position;
}

float TrefoilKnot::scale() const
{
    return m_scale;
}

void TrefoilKnot::setTheta(float theta)
{
    if (m_theta == theta)
        return;

    m_theta = theta;
    emit thetaChanged(theta);
    updateTransform();
}

void TrefoilKnot::setPhi(float phi)
{
    if (m_phi == phi)
        return;

    m_phi = phi;
    emit phiChanged(phi);
    updateTransform();
}

void TrefoilKnot::setPosition(QVector3D position)
{
    if (m_position == position)
        return;

    m_position = position;
    emit positionChanged(position);
    updateTransform();
}

void TrefoilKnot::setScale(float scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;
    emit scaleChanged(scale);
    updateTransform();
}
