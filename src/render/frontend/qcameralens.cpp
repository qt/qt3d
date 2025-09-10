// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcameralens.h"
#include "qcameralens_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QCameraLens
 * \inheaderfile Qt3DRender/QCameraLens
 * \inmodule Qt3DRender
 *
 * \brief Qt3DRender::QCameraLens specifies the projection matrix that will be used to
 * define a Camera for a 3D scene.
 *
 * \since 5.5
 */

/*!
 * \qmltype CameraLens
 * \nativetype Qt3DRender::QCameraLens
 * \inqmlmodule Qt3D.Render
 * \inherits Component3D
 * \since 5.5
 * \brief Provides the projection matrix that is used to define a Camera for 3D scene.
 */

/*!
 * \enum Qt3DRender::QCameraLens::ProjectionType
 *
 * Specifies which parameters of Qt3DRender::QCameraLens are used to compute the projection matrix.
 *
 * \value OrthographicProjection Orthogonal projection
 * \value PerspectiveProjection Perspective projection
 * \value FrustumProjection Frustum projection
 * \value CustomProjection Custom user-defined projection
 */

/*!
 * \qmlproperty enumeration CameraLens::projectionType
 *
 * Holds the type of the camera projection.
 *
 * \list
 * \li CameraLens.OrthographicProjection
 * \li CameraLens.PerspectiveProjection
 * \li CameraLens.FrustumProjection
 * \li CameraLens.CustomProjection
 * \endlist
 * \sa Qt3DRender::QCameraLens::ProjectionType
 */

/*!
 * \qmlproperty real CameraLens::nearPlane
 * Holds the current near plane of the camera lens.
 */

/*!
 * \qmlproperty real CameraLens::farPlane
 * Holds the current near plane of the camera lens.
 */

/*!
 * \qmlproperty real CameraLens::fieldOfView
 * Holds the current field of view of the camera lens in degrees.
 */

/*!
 * \qmlproperty real CameraLens::aspectRatio
 * Holds the current aspect ratio of the camera lens.
 */

/*!
 * \qmlproperty real CameraLens::left
 * Holds the current left plane of the camera lens.
 */

/*!
 * \qmlproperty real CameraLens::right
 * Holds the current right plane of the camera lens.
 */

/*!
 * \qmlproperty real CameraLens::bottom
 * Holds the current bottom plane of the camera lens.
 */

/*!
 * \qmlproperty real CameraLens::top
 * Holds the current top plane of the camera lens.
 */

/*!
 * \qmlproperty matrix4x4 CameraLens::projectionMatrix
 * Holds the current projection matrix of the camera lens.
 *
 * \note This will set the projection type to
 * Qt3DRender::QCameraLens::CustomProjection and thus ignore all other camera
 * parameters that might have been specified.
 * \readonly
 */


/*!
 * \property Qt3DRender::QCameraLens::projectionType
 *
 * Holds the type of the camera projection.
 * \sa Qt3DRender::QCameraLens::ProjectionType
 */

/*!
 * \property Qt3DRender::QCameraLens::nearPlane
 * Holds the current near plane of the camera lens.
 */

/*!
 * \property Qt3DRender::QCameraLens::farPlane
 * Holds the current near plane of the camera lens.
 */

/*!
 * \property Qt3DRender::QCameraLens::fieldOfView
 * Holds the current field of view of the camera lens.
 * \note: The return value may be undefined if the projection type is not
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */

/*!
 * \property Qt3DRender::QCameraLens::aspectRatio
 * Holds the current aspect ratio of the camera lens.
 * \note: The return value may be undefined if the projection type is not
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */

/*!
 * \property Qt3DRender::QCameraLens::left
 * Holds the current left plane of the camera lens.
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */

/*!
 * \property Qt3DRender::QCameraLens::right
 * Holds the current right plane of the camera lens.
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */

/*!
 * \property Qt3DRender::QCameraLens::bottom
 * Holds the current bottom plane of the camera lens.
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */

/*!
 * \property Qt3DRender::QCameraLens::top
 * Holds the current top plane of the camera lens.
 * \note The return value may be undefined if the projection type is
 * Qt3DRender::QCameraLens::PerspectiveProjection.
 */

/*!
 * \property Qt3DRender::QCameraLens::projectionMatrix
 * Holds the current projection matrix of the camera lens.
 * \readonly
 */

/*!
 * \property Qt3DRender::QCameraLens::exposure
 * Holds the current exposure of the camera lens.
 */

/*!
 *   \internal
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
    , m_exposure(0.0f)
{
}


void QCameraLens::viewAll(Qt3DCore::QNodeId cameraId)
{
    Q_D(QCameraLens);
    if (d->m_projectionType == PerspectiveProjection || d->m_projectionType == OrthographicProjection) {
        d->m_pendingViewAllRequest = {Qt3DCore::QNodeId::createId(), cameraId, {}};
        d->update();
    }
}

void QCameraLens::viewEntity(Qt3DCore::QNodeId entityId, Qt3DCore::QNodeId cameraId)
{
    Q_D(QCameraLens);
    if (d->m_projectionType == PerspectiveProjection || d->m_projectionType == OrthographicProjection) {
        d->m_pendingViewAllRequest = {Qt3DCore::QNodeId::createId(), cameraId, entityId};
        d->update();
    }
}

void QCameraLensPrivate::processViewAllResult(Qt3DCore::QNodeId requestId, const QVector3D &center, float radius)
{
    Q_Q(QCameraLens);
    if (!m_pendingViewAllRequest || m_pendingViewAllRequest.requestId != requestId)
        return;

    Q_EMIT q->viewSphere(center, radius);
    m_pendingViewAllRequest = {};
}

/*!
 * Constructs a QCameraLens with given \a parent
 */
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

QCameraLens::QCameraLens(QCameraLensPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
    Q_D(QCameraLens);
    d->updateOrthographicProjection();
}

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

        const bool wasBlocked = blockNotifications(true);
        emit projectionTypeChanged(projectionType);
        blockNotifications(wasBlocked);

        d->updateProjectionMatrix();
    }
}

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

    const bool wasBlocked = blockNotifications(true);
    emit nearPlaneChanged(nearPlane);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

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

    const bool wasBlocked = blockNotifications(true);
    emit farPlaneChanged(farPlane);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

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

    const bool wasBlocked = blockNotifications(true);
    emit fieldOfViewChanged(fieldOfView);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

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

    const bool wasBlocked = blockNotifications(true);
    emit aspectRatioChanged(aspectRatio);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

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

    const bool wasBlocked = blockNotifications(true);
    emit leftChanged(left);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

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

    const bool wasBlocked = blockNotifications(true);
    emit rightChanged(right);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

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

    const bool wasBlocked = blockNotifications(true);
    emit bottomChanged(bottom);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

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

    const bool wasBlocked = blockNotifications(true);
    emit topChanged(top);
    blockNotifications(wasBlocked);

    d->updateProjectionMatrix();
}

float QCameraLens::top() const
{
    Q_D(const QCameraLens);
    return d->m_top;
}

/*!
 * Sets the project matrix to \a projectionMatrix.
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

QMatrix4x4 QCameraLens::projectionMatrix() const
{
    Q_D(const QCameraLens);
    return d->m_projectionMatrix;
}

/*!
 * Sets the camera lens' \a exposure
 */
void QCameraLens::setExposure(float exposure)
{
    Q_D(QCameraLens);
    if (qFuzzyCompare(d->m_exposure, exposure))
        return;
    d->m_exposure = exposure;

    emit exposureChanged(exposure);
}

float QCameraLens::exposure() const
{
    Q_D(const QCameraLens);
    return d->m_exposure;
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qcameralens.cpp"
