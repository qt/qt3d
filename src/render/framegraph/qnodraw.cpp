// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qnodraw.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QNoDraw
    \inmodule Qt3DRender

    \brief When a Qt3DRender::QNoDraw node is present in a FrameGraph branch, this
    prevents the renderer from rendering any primitive.

    Qt3DRender::QNoDraw should be used when the FrameGraph needs to set up some render
    states or clear some buffers without requiring any mesh to be drawn. It has
    the same effect as having a Qt3DRender::QRenderPassFilter that matches none of
    available Qt3DRender::QRenderPass instances of the scene without the overhead cost
    of actually performing the filtering.

    When disabled, a Qt3DRender::QNoDraw node won't prevent the scene from
    being rendered. Toggling the enabled property is therefore a way to make a
    Qt3DRender::QNoDraw active or inactive.

    Qt3DRender::QNoDraw is usually used as a child of a
    Qt3DRendeR::QClearBuffers node to prevent from drawing the scene when there
    are multiple render passes.

    \code
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(viewport);

    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(cameraSelector);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    Qt3DRender::QNoDraw *noDraw = new Qt3DRender::QNoDraw(clearBuffers);

    Qt3DRender::QRenderPassFilter *mainPass = new Qt3DRender::QRenderPassFilter(cameraSelector);
    ....
    Qt3DRender::QRenderPassFilter *previewPass = new Qt3DRender::QRenderPassFilter(cameraSelector);
    ....
    \endcode

    \since 5.5
 */

/*!
    \qmltype NoDraw
    \instantiates Qt3DRender::QNoDraw
    \inherits FrameGraphNode
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief When a NoDraw node is present in a FrameGraph branch, this
     prevents the renderer from rendering any primitive.

    NoDraw should be used when the FrameGraph needs to set up some render
    states or clear some buffers without requiring any mesh to be drawn. It has
    the same effect as having a Qt3DRender::QRenderPassFilter that matches none
    of available Qt3DRender::QRenderPass instances of the scene without the
    overhead cost of actually performing the filtering.

    When disabled, a NoDraw node won't prevent the scene from being rendered.
    Toggling the enabled property is therefore a way to make a NoDraw active or
    inactive.

    NoDraw is usually used as a child of a ClearBuffers node to prevent from
    drawing the scene when there are multiple render passes.

    \code

    Viewport {
        CameraSelector {
            ClearBuffers {
                buffers: ClearBuffers.ColorDepthBuffer
                NoDraw { } // Prevents from drawing anything
            }
            RenderPassFilter {
                ...
            }
            RenderPassFilter {
                ...
            }
        }
    }

    \endcode
*/

/*!
  The constructor creates an instance with the specified \a parent.
 */
QNoDraw::QNoDraw(QNode *parent)
    : QFrameGraphNode(parent)
{
}

/*! \internal */
QNoDraw::~QNoDraw()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qnodraw.cpp"
