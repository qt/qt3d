// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qnopicking.h"


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QNoPicking
    \inmodule Qt3DRender
    \since 5.14

    \brief When a Qt3DRender::QNoPicking node is present in a FrameGraph
    branch, this prevents the render aspect from performing picking selection
    for the given branch.

    When disabled, a Qt3DRender::QNoPicking node won't prevent picking from
    being performed. Toggling the enabled property is therefore a way to make a
    Qt3DRender::QNoPicking active or inactive.

    When using multiple subviewports in the FrameGraph, QNoPicking can be
    useful to prevent picking conflicts between overlapping viewports or non
    visual ones. It can also be used as an optimization to prevent unnecessary
    work for hidden viewports or for sections of the scenes which don't require
    any picking.

    \code
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport();
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    Qt3DRender::QNoPicking *noPicking = new Qt3DRender::QNoPicking(cameraSelector);

    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(noPicking);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    Qt3DRender::QRenderPassFilter *mainPass = new Qt3DRender::QRenderPassFilter(cameraSelector);
    ....
    Qt3DRender::QRenderPassFilter *previewPass = new Qt3DRender::QRenderPassFilter(cameraSelector);
    ....
    \endcode
    \sa Qt3DRender::QObjectPicker, Qt3DRender::QRayCaster, Qt3DRender::QScreenRayCaster
 */

/*!
    \qmltype NoPicking
    \nativetype Qt3DRender::QNoPicking
    \inherits FrameGraphNode
    \inqmlmodule Qt3D.Render
    \since 5.14

    \brief When a NoPicking node is present in a FrameGraph branch, this
    prevents the render aspect from performing picking selection for the given
    branch.

    When disabled, a NoPicking node won't prevent picking from being performed.
    Toggling the enabled property is therefore a way to make a NoPicking active
    or inactive.

    When using multiple subviewports in the FrameGraph, NoPicking can be useful
    to prevent picking conflicts between overlapping viewports or non visual
    ones. It can also be used as an optimization to prevent unnecessary work for
    hidden viewports or for sections of the scenes which don't require any
    picking.


    \code

    Viewport {
        CameraSelector {
            NoPicking {
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
    }

    \endcode
    \sa ObjectPicker, RayCaster, ScreenRayCaster

*/

QNoPicking::QNoPicking(Qt3DCore::QNode *parent)
    : QFrameGraphNode(parent)
{
}

QNoPicking::~QNoPicking()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qnopicking.cpp"
