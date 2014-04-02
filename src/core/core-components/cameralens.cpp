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

#include "cameralens.h"
#include "cameralens_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

CameraLens::CameraLens(Node *parent) :
    Component(parent),
    d_ptr(new CameraLensPrivate(this))
{
}

void CameraLens::setProjectionType(CameraLens::ProjectionType projectionType)
{
    Q_D(CameraLens);
    if (d->m_projectionType != projectionType) {
        d->m_projectionType = projectionType;
        emit projectionTypeChanged();
    }
}

CameraLens::ProjectionType CameraLens::projectionType() const
{
    Q_D(const CameraLens);
    return d->m_projectionType;
}

void CameraLens::setOrthographicProjection( float left, float right,
                                        float bottom, float top,
                                        float nearPlane, float farPlane )
{
    Q_D(CameraLens);
    d->m_left = left;
    d->m_right = right;
    d->m_bottom = bottom;
    d->m_top = top;
    d->m_nearPlane = nearPlane;
    d->m_farPlane = farPlane;
    d->m_projectionType = OrthogonalProjection;
    d->updateOrthogonalProjection();
}

void CameraLens::setPerspectiveProjection( float fieldOfView, float aspectRatio,
                                       float nearPlane, float farPlane )
{
    Q_D(CameraLens);
    d->m_fieldOfView = fieldOfView;
    d->m_aspectRatio = aspectRatio;
    d->m_nearPlane = nearPlane;
    d->m_farPlane = farPlane;
    d->m_projectionType = PerspectiveProjection;
    d->updatePerpectiveProjection();
}

void CameraLens::setNearPlane(float nearPlane)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_nearPlane, nearPlane))
        return;
    d->m_nearPlane = nearPlane;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit nearPlaneChanged();
}

float CameraLens::nearPlane() const
{
    Q_D(const CameraLens);
    return d->m_nearPlane;
}

void CameraLens::setFarPlane(float farPlane)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_farPlane, farPlane))
        return;
    d->m_farPlane = farPlane;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit farPlaneChanged();
}

float CameraLens::farPlane() const
{
    Q_D(const CameraLens);
    return d->m_farPlane;
}

void CameraLens::setFieldOfView(float fieldOfView)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_fieldOfView, fieldOfView))
        return;
    d->m_fieldOfView = fieldOfView;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit fieldOfViewChanged();
}

float CameraLens::fieldOfView() const
{
    Q_D(const CameraLens);
    return d->m_fieldOfView;
}

void CameraLens::setAspectRatio(float aspectRatio)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_aspectRatio, aspectRatio))
        return;
    d->m_aspectRatio = aspectRatio;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit aspectRatioChanged();
}

float CameraLens::aspectRatio() const
{
    Q_D(const CameraLens);
    return d->m_aspectRatio;
}

void CameraLens::setLeft(float left)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_left, left))
        return;
    d->m_left = left;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit leftChanged();
}

float CameraLens::left() const
{
    Q_D(const CameraLens);
    return d->m_left;
}

void CameraLens::setRight(float right)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_right, right))
        return;
    d->m_right = right;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit rightChanged();
}

float CameraLens::right() const
{
    Q_D(const CameraLens);
    return d->m_right;
}

void CameraLens::setBottom(float bottom)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_bottom, bottom))
        return;
    d->m_bottom = bottom;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit bottomChanged();
}

float CameraLens::bottom() const
{
    Q_D(const CameraLens);
    return d->m_bottom;
}

void CameraLens::setTop(float top)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_top, top))
        return;
    d->m_top = top;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit topChanged();
}

float CameraLens::top() const
{
    Q_D(const CameraLens);
    return d->m_top;
}

QMatrix4x4 CameraLens::projectionMatrix() const
{
    Q_D(const CameraLens);
    return d->m_projectionMatrix;
}

} // Qt3D

QT_END_NAMESPACE
