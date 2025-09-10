// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrendertarget.h"
#include "qrendertarget_p.h"
#include "qrendertargetoutput.h"

QT_BEGIN_NAMESPACE


namespace Qt3DRender {

using namespace Qt3DCore;

/*!
    \class Qt3DRender::QRenderTarget
    \brief The QRenderTarget class encapsulates a target (usually a frame buffer
    object) which the renderer can render into.
    \since 5.7
    \inmodule Qt3DRender

    A Qt3DRender::QRenderTarget comprises of Qt3DRender::QRenderTargetOutput objects,
    which specify the the buffers the render target is rendering to. The user can
    specify MRT(Multiple Render Targets) by attaching multiple textures to different
    attachment points. The results are undefined if the user tries to attach multiple
    textures to the same attachment point. At render time, only the draw buffers specified
    in the Qt3DRender::QRenderTargetSelector are used.

 */
/*!
    \qmltype RenderTarget
    \brief The RenderTarget class encapsulates a target (usually a frame buffer
    object) which the renderer can render into.
    \since 5.7
    \inqmlmodule Qt3D.Render
    \nativetype Qt3DRender::QRenderTarget

    A RenderTarget comprises of RenderTargetOutput objects, which specify the the buffers
    the render target is rendering to. The user can specify MRT(Multiple Render Targets)
    by attaching multiple textures to different attachment points. The results are undefined
    if the user tries to attach multiple textures to the same attachment point. At render
    time, only the draw buffers specified in the RenderTargetSelector are used.
 */

/*!
    \qmlproperty list<RenderTargetOutput> RenderTarget::attachments
    Holds the attachments for the RenderTarget.
*/

/*! \internal */
QRenderTargetPrivate::QRenderTargetPrivate()
    : QComponentPrivate()
{
}

/*!
    The constructor creates a new QRenderTarget::QRenderTarget instance with
    the specified \a parent.
 */
QRenderTarget::QRenderTarget(QNode *parent)
    : QComponent(*new QRenderTargetPrivate, parent)
{
}

/*! \internal */
QRenderTarget::~QRenderTarget()
{
}

/*! \internal */
QRenderTarget::QRenderTarget(QRenderTargetPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

/*!
    Adds a chosen output via \a output.
 */
void QRenderTarget::addOutput(QRenderTargetOutput *output)
{
    Q_D(QRenderTarget);
    if (output && !d->m_outputs.contains(output)) {
        d->m_outputs.append(output);

        // Ensures proper bookkeeping
        d->registerDestructionHelper(output, &QRenderTarget::removeOutput, d->m_outputs);

        if (!output->parent())
            output->setParent(this);

        d->update();
    }
}

/*!
    Removes a chosen output via \a output.
 */
void QRenderTarget::removeOutput(QRenderTargetOutput *output)
{
    Q_D(QRenderTarget);

    if (!d->m_outputs.removeOne(output))
        return;
    d->update();
    // Remove bookkeeping connection
    d->unregisterDestructionHelper(output);
}

/*!
    \return the chosen outputs.
 */
QList<QRenderTargetOutput *> QRenderTarget::outputs() const
{
    Q_D(const QRenderTarget);
    return d->m_outputs;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrendertarget.cpp"
