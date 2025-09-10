// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qfrustumculling.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
/*!
    \class Qt3DRender::QFrustumCulling
    \inmodule Qt3DRender
    \since 5.7
    \ingroup framegraph
    \brief Enable frustum culling for the FrameGraph.

    A QFrustumCulling class enables frustum culling of the drawable entities based on
    the camera view and QGeometry bounds of the entities. If QFrustumCulling is present in
    the FrameGraph, only the entities whose QGeometry bounds intersect with the camera
    frustum, i.e. the view of the camera, are drawn. If QFrustumCulling is not present,
    all drawable entities will be drawn. The camera is selected by a QCameraSelector
    frame graph node in the current hierarchy. Frustum culling can save a lot of GPU
    processing time when the rendered scene is complex.

    \sa QCameraSelector
 */

/*!
    \qmltype FrustumCulling
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QFrustumCulling
    \inherits FrameGraphNode
    \since 5.7
    \brief Enable frustum culling for the FrameGraph.

    A FrustumCulling type enables frustum culling of the drawable entities based on
    the camera view and Geometry bounds of the entities. If FrustumCulling is present in
    the FrameGraph, only the entities whose Geometry bounds intersect with the camera
    frustum, i.e. the view of the camera, are drawn. If FrustumCulling is not present,
    all drawable entities will be drawn. The camera is selected by a CameraSelector
    frame graph node in the current hierarchy. Frustum culling can save a lot of GPU
    processing time when the rendered scene is complex.

    \sa CameraSelector
*/

/*!
    The constructor creates an instance with the specified \a parent.
 */
QFrustumCulling::QFrustumCulling(Qt3DCore::QNode *parent)
    : QFrameGraphNode(parent)
{
}

/*! \internal */
QFrustumCulling::~QFrustumCulling()
{
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qfrustumculling.cpp"

