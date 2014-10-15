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
    d->updateProjectionMatrix();
}

void QCameraLensPrivate::copy(const QNodePrivate *ref)
{
    QComponentPrivate::copy(ref);
    const QCameraLensPrivate *lens = static_cast<const QCameraLensPrivate *>(ref);
    m_projectionType = lens->m_projectionType;
    m_nearPlane = lens->m_nearPlane;
    m_farPlane = lens->m_farPlane;
    m_fieldOfView = lens->m_fieldOfView;
    m_aspectRatio = lens->m_aspectRatio;
    m_left = lens->m_left;
    m_right = lens->m_right;
    m_bottom = lens->m_bottom;
    m_top = lens->m_top;
    m_projectionMatrix = lens->m_projectionMatrix;
}

QCameraLens::QCameraLens(QCameraLensPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
    Q_D(QCameraLens);
    d->updateOrthogonalProjection();
}

void QCameraLens::setProjectionType(QCameraLens::ProjectionType projectionType)
{
    Q_D(QCameraLens);
    if (d->m_projectionType != projectionType) {
        d->m_projectionType = projectionType;
        emit projectionTypeChanged();
        d->updateProjectionMatrix();
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
    d->updateProjectionMatrix();
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
    d->updateProjectionMatrix();
}

void QCameraLens::setNearPlane(float nearPlane)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_nearPlane, nearPlane))
        return;
    d->m_nearPlane = nearPlane;
    emit nearPlaneChanged();
    d->updateProjectionMatrix();
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
    emit farPlaneChanged();
    d->updateProjectionMatrix();
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
    emit fieldOfViewChanged();
    d->updateProjectionMatrix();
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
    emit aspectRatioChanged();
    d->updateProjectionMatrix();
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
    emit leftChanged();
    d->updateProjectionMatrix();
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
    emit rightChanged();
    d->updateProjectionMatrix();
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
    emit bottomChanged();
    d->updateProjectionMatrix();
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
    emit topChanged();
    d->updateProjectionMatrix();
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
