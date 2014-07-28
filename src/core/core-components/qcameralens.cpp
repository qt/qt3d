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

#include "qcameralens.h"
#include "qcameralens_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QCameraLensPrivate::QCameraLensPrivate(QCameraLens *qq)
  : QComponentPrivate(qq)
  , m_projectionType(QCameraLens::OrthogonalProjection)
  , m_nearPlane(0.1f)
  , m_farPlane(1024.0f)
  , m_fieldOfView(25.0f)
  , m_aspectRatio(1.0f)
  , m_left(-0.5f)
  , m_right(0.5f)
  , m_bottom(-0.5f)
  , m_top(0.5f)
{
}


QCameraLens::QCameraLens(QNode *parent)
    : QComponent(*new QCameraLensPrivate(this), parent)
{
    Q_D(QCameraLens);
    d->updateOrthogonalProjection();
}

void QCameraLens::copy(const QNode *ref)
{
    Q_D(QCameraLens);
    QComponent::copy(ref);
    const QCameraLens *lens = qobject_cast<const QCameraLens *>(ref);
    if (lens != Q_NULLPTR) {
        d->m_projectionType = lens->projectionType();
        d->m_nearPlane = lens->nearPlane();
        d->m_farPlane = lens->farPlane();
        d->m_fieldOfView = lens->fieldOfView();
        d->m_aspectRatio = lens->aspectRatio();
        d->m_left = lens->left();
        d->m_right = lens->right();
        d->m_bottom = lens->bottom();
        d->m_top = lens->top();
    }
}

QCameraLens::QCameraLens(QCameraLensPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
    Q_D(QCameraLens);
    d->updateOrthogonalProjection();
}

QCameraLens *QCameraLens::doClone(QNode *clonedParent) const
{
    return new QCameraLens(clonedParent);
}

void QCameraLens::setProjectionType(QCameraLens::ProjectionType projectionType)
{
    Q_D(QCameraLens);
    if (d->m_projectionType != projectionType) {
        d->m_projectionType = projectionType;
        emit projectionTypeChanged();
    }
}

QCameraLens::ProjectionType QCameraLens::projectionType() const
{
    Q_D(const QCameraLens);
    return d->m_projectionType;
}

void QCameraLens::setOrthographicProjection( float left, float right,
                                        float bottom, float top,
                                        float nearPlane, float farPlane )
{
    Q_D(QCameraLens);
    d->m_left = left;
    d->m_right = right;
    d->m_bottom = bottom;
    d->m_top = top;
    d->m_nearPlane = nearPlane;
    d->m_farPlane = farPlane;
    d->m_projectionType = OrthogonalProjection;
    d->updateOrthogonalProjection();
}

void QCameraLens::setPerspectiveProjection( float fieldOfView, float aspectRatio,
                                       float nearPlane, float farPlane )
{
    Q_D(QCameraLens);
    d->m_fieldOfView = fieldOfView;
    d->m_aspectRatio = aspectRatio;
    d->m_nearPlane = nearPlane;
    d->m_farPlane = farPlane;
    d->m_projectionType = PerspectiveProjection;
    d->updatePerpectiveProjection();
}

void QCameraLens::setNearPlane(float nearPlane)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_nearPlane, nearPlane))
        return;
    d->m_nearPlane = nearPlane;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit nearPlaneChanged();
}

float QCameraLens::nearPlane() const
{
    Q_D(const QCameraLens);
    return d->m_nearPlane;
}

void QCameraLens::setFarPlane(float farPlane)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_farPlane, farPlane))
        return;
    d->m_farPlane = farPlane;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit farPlaneChanged();
}

float QCameraLens::farPlane() const
{
    Q_D(const QCameraLens);
    return d->m_farPlane;
}

void QCameraLens::setFieldOfView(float fieldOfView)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_fieldOfView, fieldOfView))
        return;
    d->m_fieldOfView = fieldOfView;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit fieldOfViewChanged();
}

float QCameraLens::fieldOfView() const
{
    Q_D(const QCameraLens);
    return d->m_fieldOfView;
}

void QCameraLens::setAspectRatio(float aspectRatio)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_aspectRatio, aspectRatio))
        return;
    d->m_aspectRatio = aspectRatio;
    if (d->m_projectionType == PerspectiveProjection)
        d->updatePerpectiveProjection();
    emit aspectRatioChanged();
}

float QCameraLens::aspectRatio() const
{
    Q_D(const QCameraLens);
    return d->m_aspectRatio;
}

void QCameraLens::setLeft(float left)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_left, left))
        return;
    d->m_left = left;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit leftChanged();
}

float QCameraLens::left() const
{
    Q_D(const QCameraLens);
    return d->m_left;
}

void QCameraLens::setRight(float right)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_right, right))
        return;
    d->m_right = right;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit rightChanged();
}

float QCameraLens::right() const
{
    Q_D(const QCameraLens);
    return d->m_right;
}

void QCameraLens::setBottom(float bottom)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_bottom, bottom))
        return;
    d->m_bottom = bottom;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit bottomChanged();
}

float QCameraLens::bottom() const
{
    Q_D(const QCameraLens);
    return d->m_bottom;
}

void QCameraLens::setTop(float top)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_top, top))
        return;
    d->m_top = top;
    if (d->m_projectionType == OrthogonalProjection)
        d->updateOrthogonalProjection();
    emit topChanged();
}

float QCameraLens::top() const
{
    Q_D(const QCameraLens);
    return d->m_top;
}

QMatrix4x4 QCameraLens::projectionMatrix() const
{
    Q_D(const QCameraLens);
    return d->m_projectionMatrix;
}

} // Qt3D

QT_END_NAMESPACE
