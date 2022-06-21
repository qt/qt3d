// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qcameraselector.h"
#include "qcameraselector_p.h"
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qentity_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QCameraSelector
    \inmodule Qt3DRender
    \since 5.5
    \ingroup framegraph

    \brief Class to allow for selection of camera to be used.

    A Qt3DRender::QCameraSelector can be used to select the camera, which is used
    by the FrameGraph when drawing the entities.
 */

/*!
    \qmltype CameraSelector
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QCameraSelector
    \inherits FrameGraphNode
    \since 5.5
    \brief Class to allow for selection of camera to be used.

    A CameraSelector can be used to select the camera, which is used
    by the FrameGraph when drawing the entities.
*/

/*!
    \qmlproperty Entity Qt3D.Render::CameraSelector::camera

    Holds the currently selected camera.
*/

/*!
    \property Qt3DRender::QCameraSelector::camera

    Holds the currently selected camera.
*/


/*! \internal */
QCameraSelector::QCameraSelector(QCameraSelectorPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

QCameraSelectorPrivate::QCameraSelectorPrivate()
    : QFrameGraphNodePrivate()
    , m_camera(nullptr)
{
}

/*!
  The constructor creates an instance with the specified \a parent.
 */
QCameraSelector::QCameraSelector(Qt3DCore::QNode *parent)
    :   QFrameGraphNode(*new QCameraSelectorPrivate, parent)
{
}

/*! \internal */
QCameraSelector::~QCameraSelector()
{
}

void QCameraSelector::setCamera(Qt3DCore::QEntity *camera)
{
    Q_D(QCameraSelector);
    if (d->m_camera != camera) {

        if (d->m_camera)
            d->unregisterDestructionHelper(d->m_camera);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (camera && !camera->parent())
            camera->setParent(this);
        d->m_camera = camera;

        // Ensures proper bookkeeping
        if (d->m_camera)
            d->registerDestructionHelper(d->m_camera, &QCameraSelector::setCamera, d->m_camera);

        emit cameraChanged(camera);
    }
}

Qt3DCore::QEntity *QCameraSelector::camera() const
{
    Q_D(const QCameraSelector);
    return d->m_camera;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qcameraselector.cpp"
