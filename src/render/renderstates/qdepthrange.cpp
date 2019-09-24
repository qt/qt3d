/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdepthrange.h"
#include "qdepthrange_p.h"
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

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

Qt3DCore::QNodeCreatedChangeBasePtr QDepthRange::createNodeCreationChange() const
{
    auto creationChange = QRenderStateCreatedChangePtr<QDepthRangeData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QDepthRange);
    data.nearValue = d->m_nearValue;
    data.farValue = d->m_farValue;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
