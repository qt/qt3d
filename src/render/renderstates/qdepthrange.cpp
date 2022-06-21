// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdepthrange.h"
#include "qdepthrange_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QDepthRange
    \inmodule Qt3DRender
    \since 5.14
    \ingroup renderstates
    \brief Enables remapping depth values written into the depth buffer.

    By default, OpenGL writes scene depth information into the depth buffer in
    the range [0.0, 1.0] with 0.0 corresponding to the near clip plane and 1.0 to
    the far clip plane. QDepthRange allows mapping these values into a different
    range so parts of the scene are always rendered in front of or behind other
    parts.  Valid values for near and far are between 0 and 1.
 */

/*!
    \qmltype DepthRange
    \instantiates Qt3DRender::QDepthRange
    \inherits RenderState
    \inqmlmodule Qt3D.Render
    \ingroup renderstates
    \since 5.14
    \brief Enables remapping depth values written into the depth buffer.

    By default, OpenGL writes scene depth information into the depth buffer in
    the range [0.0, 1.0] corresponding to the near and far clip planes.
    QDepthRange allows mapping these values into a different range. For example
    setting the range [0.0, 0.5] will map the rendered scene into the depth
    buffer such that objects at the near clip plane have depth value of 0.0 and
    objects at the far clip plane have a depth value of 0.5. This allows
    rendering parts of the scene always in front of or behind other parts.
*/

/*!
    \qmlproperty real QDepthRange::nearValue
    The depth buffer value corresponding to the near clip plane. Valid values for are
    between 0 and 1.
*/

/*!
    \qmlproperty real QDepthRange::farValue
    The depth buffer value corresponding to the far clip plane. Valid values for are
    between 0 and 1.
*/

/*!
    \property QDepthRange::nearValue
    The depth buffer value corresponding to the near clip plane. Valid values for are
    between 0 and 1.
*/

/*!
    \property QDepthRange::farValue
    The depth buffer value corresponding to the far clip plane. Valid values for are
    between 0 and 1.
*/

QDepthRange::QDepthRange(QNode *parent)
    : QRenderState(*new QDepthRangePrivate(), parent)
{
}

/*! \internal */
QDepthRange::~QDepthRange()
{
}

double QDepthRange::nearValue() const
{
    Q_D(const QDepthRange);
    return d->m_nearValue;
}

double QDepthRange::farValue() const
{
    Q_D(const QDepthRange);
    return d->m_farValue;
}

void QDepthRange::setNearValue(double value)
{
    Q_D(QDepthRange);
    if (value != d->m_nearValue) {
        d->m_nearValue = value;
        Q_EMIT nearValueChanged(value);
    }
}

void QDepthRange::setFarValue(double value)
{
    Q_D(QDepthRange);
    if (value != d->m_farValue) {
        d->m_farValue = value;
        Q_EMIT farValueChanged(value);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qdepthrange.cpp"
