/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcameralens.h"
#include "qcameralens_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \internal
*/
QCameraLensPrivate::QCameraLensPrivate()
    : Qt3DCore::QComponentPrivate()
    , m_projectionType(QCameraLens::PerspectiveProjection)
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
    : Qt3DCore::QComponent(*new QCameraLensPrivate, parent)
{
    Q_D(QCameraLens);
    d->updateProjectionMatrix();
}

/*! \internal */
QCameraLens::~QCameraLens()
{
}

/*! \class Qt3DRender::QCameraLens
 *  \inmodule Qt3DCore
 *
 * \brief Qt3DRender::QCameraLens specifies the projection matrix that will be used to
 * define a Camera for a 3D scene.
 *
 *  \since 5.5
 */
QCameraLens::QCameraLens(QCameraLensPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
    Q_D(QCameraLens);
    d->updateOrthographicProjection();
}

/*!
 * \enum Qt3DRender::QCameraLens::ProjectionType
 *
 * Specifies which parameters of Qt3DRender::QCameraLens are used to compute the projection matrix.
 *
 * \value OrthographicProjection
 * \value PerspectiveProjection
 * \value FrustumProjection
 * \value CustomProjection
 */

/*!
 * Sets the lens' projection type \a projectionType.
 *
 * \note Qt3DRender::QCameraLens::Frustum and
 * Qt3DRender::QCameraLens::PerspectiveProjection are two different ways of
 * specifying the same projection.
 */
void QCameraLens::setProjectionType(QCameraLens::ProjectionType projectionType)
{
    Q_D(QCameraLens);
    if (d->m_projectionType != projectionType) {
        d->m_projectionType = projectionType;
        emit projectionTypeChanged(projectionType);
        d->updateProjectionMatrix();
    }
}

/*!
 * Returns the lens' projection type.
 */
QCameraLens::ProjectionType QCameraLens::projectionType() const
{
    Q_D(const QCameraLens);
    return d->m_projectionType;
}

/*!
 * Defines an orthographic projection based on \a left, \a right, \a bottom, \a
 * top, \a nearPlane, \a farPlane.
 */
void QCameraLens::setOrthographicProjection(float left, float right,
                                            float bottom, float top,
                                            float nearPlane, float farPlane)
{
    Q_D(QCameraLens);
    bool block = blockNotifications(true);
    setLeft(left);
    setRight(right);
    setBottom(bottom);
    setTop(top);
    setNearPlane(nearPlane);
    setFarPlane(farPlane);
    setProjectionType(OrthographicProjection);
    blockNotifications(block);
    d->updateProjectionMatrix();
}

/*!
 * Defines an orthographic projection based on \a left, \a right, \a bottom, \a
 * top, \a nearPlane, \a farPlane.
 */
void QCameraLens::setFrustumProjection(float left, float right,
                                       float bottom, float top,
                                       float nearPlane, float farPlane)
{
    Q_D(QCameraLens);
    bool block = blockNotifications(true);
    setLeft(left);
    setRight(right);
    setBottom(bottom);
    setTop(top);
    setNearPlane(nearPlane);
    setFarPlane(farPlane);
    setProjectionType(FrustumProjection);
    blockNotifications(block);
    d->updateProjectionMatrix();
}

/*!
 * Defines a perspective projection based on \a fieldOfView, \a aspectRatio, \a
 * nearPlane, \a farPlane.
 */
void QCameraLens::setPerspectiveProjection(float fieldOfView, float aspectRatio,
                                           float nearPlane, float farPlane)
{
    Q_D(QCameraLens);
    bool block = blockNotifications(true);
    setFieldOfView(fieldOfView);
    setAspectRatio(aspectRatio);
    setNearPlane(nearPlane);
    setFarPlane(farPlane);
    setProjectionType(PerspectiveProjection);
    blockNotifications(block);
    d->updateProjectionMatrix();
}

/*!
 * Sets the projection's near plane to \a nearPlane. This triggers a projection
 * matrix update.
 */
void QCameraLens::setNearPlane(float nearPlane)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_nearPlane, nearPlane))
        return;
    d->m_nearPlane = nearPlane;
    emit nearPlaneChanged(nearPlane);
    d->updateProjectionMatrix();
}

/*!
 * Returns the projection's near plane.
 */
float QCameraLens::nearPlane() const
{
    Q_D(const QCameraLens);
    return d->m_nearPlane;
}

/*!
 * Sets the projection's far plane to \a farPlane. This triggers a projection
 * matrix update.
 */
void QCameraLens::setFarPlane(float farPlane)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_farPlane, farPlane))
        return;
    d->m_farPlane = farPlane;
    emit farPlaneChanged(farPlane);
    d->updateProjectionMatrix();
}

/*!
 * Returns the projection's far plane.
 */
float QCameraLens::farPlane() const
{
    Q_D(const QCameraLens);
    return d->m_farPlane;
}

/*!
 * Sets the projection's field of view to \a fieldOfView degrees. This triggers
 * a projection matrix update.
 *
 * \note this has no effect if the projection type is not
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
void QCameraLens::setFieldOfView(float fieldOfView)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_fieldOfView, fieldOfView))
        return;
    d->m_fieldOfView = fieldOfView;
    emit fieldOfViewChanged(fieldOfView);
    d->updateProjectionMatrix();
}

/*!
 * Returns the projection's field of view in degrees.
 *
 * \note: The return value may be undefined if the projection type is not
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
float QCameraLens::fieldOfView() const
{
    Q_D(const QCameraLens);
    return d->m_fieldOfView;
}

/*!
 * Sets the projection's aspect ratio to \a aspectRatio. This triggers a projection
 * matrix update.
 *
 * \note this has no effect if the projection type is not
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
void QCameraLens::setAspectRatio(float aspectRatio)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_aspectRatio, aspectRatio))
        return;
    d->m_aspectRatio = aspectRatio;
    emit aspectRatioChanged(aspectRatio);
    d->updateProjectionMatrix();
}

/*!
 * Returns the projection's aspect ratio.
 *
 * \note: The return value may be undefined if the projection type is not
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
float QCameraLens::aspectRatio() const
{
    Q_D(const QCameraLens);
    return d->m_aspectRatio;
}

/*!
 * Sets the projection's lower left window coordinate to \a left. This
 * triggers a projection matrix update.
 *
 * \note this has no effect if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
void QCameraLens::setLeft(float left)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_left, left))
        return;
    d->m_left = left;
    emit leftChanged(left);
    d->updateProjectionMatrix();
}

/*!
 * Returns the lower left window coordinate of the projection.
 *
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
float QCameraLens::left() const
{
    Q_D(const QCameraLens);
    return d->m_left;
}

/*!
 * Sets the projection's upper right window coordinate to \a right. This triggers
 * a projection matrix update.
 *
 * \note this has no effect if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
void QCameraLens::setRight(float right)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_right, right))
        return;
    d->m_right = right;
    emit rightChanged(right);
    d->updateProjectionMatrix();
}

/*!
 * Returns the upper right window coordinate of the projection.
 *
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
float QCameraLens::right() const
{
    Q_D(const QCameraLens);
    return d->m_right;
}

/*!
 * Sets the projection's bottom window coordinate to \a bottom. This triggers a
 * projection matrix update.
 *
 * \note this has no effect if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
void QCameraLens::setBottom(float bottom)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_bottom, bottom))
        return;
    d->m_bottom = bottom;
    emit bottomChanged(bottom);
    d->updateProjectionMatrix();
}

/*!
 * Returns the bottom window coordinate of the projection.
 *
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
float QCameraLens::bottom() const
{
    Q_D(const QCameraLens);
    return d->m_bottom;
}

/*!
 * Sets the projection's top window coordinate to \a top. This triggers a
 * projection matrix update.
 *
 * \note this has no effect if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
void QCameraLens::setTop(float top)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_top, top))
        return;
    d->m_top = top;
    emit topChanged(top);
    d->updateProjectionMatrix();
}

/*!
 * Returns the bottom window coordinate of the projection.
 *
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */
float QCameraLens::top() const
{
    Q_D(const QCameraLens);
    return d->m_top;
}

/*!
 * Sets the project matrix.
 *
 * \note This will set the projection type to Qt3DRender::QCameraLens::CustomProjection and thus
 * ignore all other camera parameters that might have been specified.
 */
void QCameraLens::setProjectionMatrix(const QMatrix4x4 &projectionMatrix)
{
    Q_D(QCameraLens);
    setProjectionType(CustomProjection);
    if (qFuzzyCompare(d->m_projectionMatrix, projectionMatrix))
        return;
    d->m_projectionMatrix = projectionMatrix;
    emit projectionMatrixChanged(projectionMatrix);
}

/*!
 * Returns the projection matrix.
 */
QMatrix4x4 QCameraLens::projectionMatrix() const
{
    Q_D(const QCameraLens);
    return d->m_projectionMatrix;
}

Qt3DCore::QNodeCreatedChangeBasePtr QCameraLens::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QCameraLensData>::create(this);
    auto &data = creationChange->data;
    data.projectionMatrix = d_func()->m_projectionMatrix;
    return creationChange;
}

} // Qt3DRender

/*!
    \qmltype CameraLens
    \instantiates Qt3DRender::QCameraLens
    \inqmlmodule Qt3D.Core
    \inherits Component3D
    \since 5.5
    \brief Provides the projection matrix that is used to define a Camera for 3D scene.
*/

/*!
    \qmlproperty enumeration Qt3DCore::CameraLens::projectionType

    Holds the type of the camera projection (orthogonal or perspective).

    \value CameraLens.OrthographicProjection Orthogonal projection
    \value CameraLens.PerspectiveProjection Perspective projection
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::nearPlane
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::farPlane
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::fieldOfView
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::aspectRatio
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::left
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::right
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::bottom
*/

/*!
    \qmlproperty float Qt3DCore::CameraLens::top
*/

/*!
    \qmlproperty matrix4x4 Qt3DCore::CameraLens::projectionMatrix
    \readonly
*/

QT_END_NAMESPACE
