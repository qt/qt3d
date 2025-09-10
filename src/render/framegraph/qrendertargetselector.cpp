// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrendertargetselector.h"
#include "qrendertargetselector_p.h"
#include <Qt3DRender/qrendertarget.h>
#include <Qt3DRender/private/qrenderpass_p.h>

QT_BEGIN_NAMESPACE


namespace Qt3DRender {
using namespace Qt3DCore;

/*!
    \class Qt3DRender::QRenderTargetSelector
    \inmodule Qt3DRender
    \since 5.7
    \brief Provides a way of specifying a render target.

    A Qt3DRender::QRenderTargetSelector is used to select active Qt3DRender::QRenderTarget
    for the FrameGraph. When QRenderTargetSelector is present in the FrameGraph,
    the rendering is directed into QTexture objects or draw buffers instead of the surface
    specified in the Qt3DRender::QRenderSurfaceSelector.
 */

/*!
    \qmltype RenderTargetSelector
    \inqmlmodule Qt3D.Render
    \since 5.7
    \nativetype Qt3DRender::QRenderTargetSelector
    \inherits FrameGraphNode
    \brief Provides a way of specifying a render target.

    A RenderTargetSelector is used to select active RenderTarget
    for the FrameGraph. When RenderTargetSelector is present in the FrameGraph,
    the rendering is directed into Texture objects or draw buffers instead of the surface
    specified in the RenderSurfaceSelector.
 */

QRenderTargetSelectorPrivate::QRenderTargetSelectorPrivate()
    : QFrameGraphNodePrivate()
    , m_target(nullptr)
{
}

/*!
    Constructs QRenderTargetSelector with given \a parent.
 */
QRenderTargetSelector::QRenderTargetSelector(QNode *parent)
    : QFrameGraphNode(*new QRenderTargetSelectorPrivate, parent)
{
}

/*! \internal */
QRenderTargetSelector::~QRenderTargetSelector()
{
}

/*!
    \property Qt3DRender::QRenderTargetSelector::target
    Holds the current render target
 */

/*! \qmlproperty RenderTarget Qt3D.Render::RenderTargetSelector::target

    Holds the current render target
 */
void QRenderTargetSelector::setTarget(QRenderTarget *target)
{
    Q_D(QRenderTargetSelector);
    if (d->m_target != target) {

        if (d->m_target)
            d->unregisterDestructionHelper(d->m_target);

        // For inline declaration cases
        if (target != nullptr && !target->parent())
            target->setParent(this);

        d->m_target = target;

        if (d->m_target)
            d->registerDestructionHelper(d->m_target, &QRenderTargetSelector::setTarget, d->m_target);

        emit targetChanged(target);
    }
}

QRenderTarget *QRenderTargetSelector::target() const
{
    Q_D(const QRenderTargetSelector);
    return d->m_target;
}

/*! \internal */
QRenderTargetSelector::QRenderTargetSelector(QRenderTargetSelectorPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrendertargetselector.cpp"
