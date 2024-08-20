// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdebugoverlay.h"
#include "qdebugoverlay_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QDebugOverlay
    \inmodule Qt3DRender
    \ingroup framegraph
    \brief Enables a visual overlay with scene details.

    Placing a QDebugOverlay in one branch of a custom framegraph will cause an overlay
    with scene details to be rendered along with the scene.

    The standard QForwardRenderer includes a QDebugOverlay node that can be enabled via
    the QForwardRenderer::showDebugOverlay property.

*/

/*!
    \qmltype DebugOverlay
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QDebugOverlay
    \inherits FrameGraphNode
    \since 2.16
    \brief Enables a visual overlay with scene details.

    Placing a DebugOverlay in one branch of a custom framegraph will cause an overlay
    with scene details to be rendered along with the scene.

    The standard ForwardRenderer includes a DebugOverlay node that can be enabled via
    the ForwardRenderer::showDebugOverlay property.
*/

/*!
 * \internal
 */
QDebugOverlayPrivate::QDebugOverlayPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
 * \internal
 */
QDebugOverlayPrivate::~QDebugOverlayPrivate() = default;

/*!
 * The constructor creates an instance with the specified \a parent.
 */
QDebugOverlay::QDebugOverlay(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QDebugOverlayPrivate, parent)
{
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qdebugoverlay.cpp"
