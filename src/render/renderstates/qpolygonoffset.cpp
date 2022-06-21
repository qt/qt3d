// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qpolygonoffset.h"
#include "qpolygonoffset_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QPolygonOffset
    \brief The QPolygonOffset class sets the scale and steps to calculate depth
    values for polygon offsets.
    \since 5.7
    \inmodule Qt3DRender
    \ingroup renderstates

    A QPolygonOffset class adds an offset to the fragment depth value prior to
    depth test and depth write. The offset can be used to avoid z-fighting when
    rendering polygons with very close depth values such as decals.
 */

/*!
    \qmltype PolygonOffset
    \brief The PolygonOffset type sets the scale and steps to calculate depth
    values for polygon offsets.
    \since 5.7
    \inqmlmodule Qt3D.Render
    \ingroup renderstates
    \inherits RenderState
    \instantiates Qt3DRender::QPolygonOffset

    A PolygonOffset type adds an offset to the fragment depth value prior to
    depth test and depth write. The offset can be used to avoid z-fighting when
    rendering polygons with very close depth values such as decals.
 */

/*!
    \qmlproperty real PolygonOffset::scaleFactor
    Holds the scale factor used to create a variable depth offset for
    each polygon. Default value is 0.
*/

/*!
    \qmlproperty real PolygonOffset::depthSteps
    Holds the units that create constant depth offsets. Default value is 0.
*/

/*!
    \property QPolygonOffset::scaleFactor
    Holds the scale factor used to create a variable depth offset for
    each polygon. Default value is 0.
*/

/*!
    \property QPolygonOffset::depthSteps
    Holds the units that create constant depth offsets. Default value is 0.
*/

/*!
    The constructor creates a new QPolygonOffset::QPolygonOffset instance
    with the specified \a parent
 */
QPolygonOffset::QPolygonOffset(QNode *parent)
    : QRenderState(*new QPolygonOffsetPrivate, parent)
{
}

/*! \internal */
QPolygonOffset::~QPolygonOffset()
{
}

float QPolygonOffset::scaleFactor() const
{
    Q_D(const QPolygonOffset);
    return d->m_scaleFactor;
}

void QPolygonOffset::setScaleFactor(float scaleFactor)
{
    Q_D(QPolygonOffset);
    if (d->m_scaleFactor != scaleFactor) {
        d->m_scaleFactor = scaleFactor;
        emit scaleFactorChanged(d->m_scaleFactor);
    }
}

float QPolygonOffset::depthSteps() const
{
    Q_D(const QPolygonOffset);
    return d->m_depthSteps;
}

void QPolygonOffset::setDepthSteps(float depthSteps)
{
    Q_D(QPolygonOffset);
    if (d->m_depthSteps != depthSteps) {
        d->m_depthSteps = depthSteps;
        emit depthStepsChanged(d->m_depthSteps);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qpolygonoffset.cpp"
