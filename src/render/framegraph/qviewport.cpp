// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qviewport.h"
#include "qviewport_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QViewportPrivate::QViewportPrivate()
    : QFrameGraphNodePrivate()
    , m_normalizedRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f))
    , m_gamma(2.2f)
{
}

/*!
    \class Qt3DRender::QViewport
    \inmodule Qt3DRender
    \brief A viewport on the Qt3D Scene.
    \since 5.7

    \inherits Qt3DRender::QFrameGraphNode

    Qt3DRender::QViewport of the scene specifies at which portion of the render surface Qt3D
    is rendering to. Area outside the viewport is left untouched. It also controls global parameters
    to the rendering in that viewport like gamma.
 */

/*!
    \qmltype Viewport
    \inqmlmodule Qt3D.Render
    \since 5.7
    \inherits FrameGraphNode
    \nativetype Qt3DRender::QViewport
    \brief A viewport on the Qt3D Scene.

    Viewport of the scene specifies at which portion of the render surface Qt3D is
    rendering to. Area outside the viewport is left untouched. It also controls global parameters
    to the rendering in that viewport like gamma.
 */

/*!
    \qmlproperty rect Viewport::normalizedRect

    Specifies the normalised rectangle for the viewport, i.e. the viewport rectangle
    is specified relative to the render surface size. Whole surface sized viewport
    is specified as [0.0, 0.0, 1.0, 1.0], which is the default.
 */

/*!
    \qmlproperty real Viewport::gamma

    Specifies the gamma factor for the viewport. The default is 2.2 which should give proper result on most screens.
 */

/*!
    Constructs QViewport with given \a parent.
 */
QViewport::QViewport(QNode *parent)
    : QFrameGraphNode(*new QViewportPrivate, parent)
{
}

/*! \internal */
QViewport::~QViewport()
{
}

/*! \internal */
QViewport::QViewport(QViewportPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}


QRectF QViewport::normalizedRect() const
{
    Q_D(const QViewport);
    return d->m_normalizedRect;
}

float QViewport::gamma() const
{
    Q_D(const QViewport);
    return d->m_gamma;
}

/*!
    \property Qt3DRender::QViewport::normalizedRect

    Specifies the normalised rectangle for the viewport, i.e. the viewport rectangle
    is specified relative to the render surface size. Whole surface sized viewport
    is specified as [0.0, 0.0, 1.0, 1.0], which is the default.
 */
void QViewport::setNormalizedRect(const QRectF &normalizedRect)
{
    Q_D(QViewport);
    if (normalizedRect != d->m_normalizedRect) {
        d->m_normalizedRect = normalizedRect;
        emit normalizedRectChanged(normalizedRect);
    }
}

/*!
    \property Qt3DRender::QViewport::gamma

    Specifies the gamma factor for the viewport. The default is 2.2 which should give proper result on most screens.
 */
void QViewport::setGamma(float gamma)
{
    Q_D(QViewport);
    if (gamma != d->m_gamma) {
        d->m_gamma = gamma;
        emit gammaChanged(gamma);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qviewport.cpp"
