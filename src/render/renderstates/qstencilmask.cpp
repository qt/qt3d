/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qstencilmask.h"
#include "qstencilmask_p.h"
#include <Qt3DRender/private/qrenderstatecreatedchange_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QStencilMask
    \brief The QStencilMask class controls the front and back writing of
    individual bits in the stencil planes.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    A Qt3DRender::QStencilMask class specifies a write mask for the stencil values
    after the stencil test. Mask can be specified separately for the front-facing
    and back-facing polygons. The fragment stencil value is and'd with the mask
    before it is written to the stencil buffer.

    \sa Qt3DRender::QStencilTest
 */

/*!
    \qmltype StencilMask
    \brief The StencilMask type controls the front and back writing of
    individual bits in the stencil planes.
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QStencilMask
    \inherits RenderState

    A StencilMask type specifies the mask for the stencil test. Mask can be specified
    separately for the front-facing and back-facing polygons. The stencil test reference
    value and stencil buffer value gets and'd with the mask prior to applying stencil function.

    \sa StencilTest
 */

/*!
    \qmlproperty int StencilMask::frontOutputMask
    Holds the write mask for the fragment stencil values for front-facing polygons.
*/

/*!
    \qmlproperty int StencilMask::backOutputMask
    Holds the write mask for the fragment stencil values for back-facing polygons.
*/

/*!
    \property QStencilMask::frontOutputMask
    Holds the write mask for the fragment stencil values for front-facing polygons.
*/

/*!
    \property QStencilMask::backOutputMask
    Holds the write mask for the fragment stencil values for back-facing polygons.
*/

/*!
    The constructor creates a new QStencilMask::QStencilMask instance with the
    specified \a parent.
 */
QStencilMask::QStencilMask(QNode *parent)
    : QRenderState(*new QStencilMaskPrivate(), parent)
{
}

/*! \internal */
QStencilMask::~QStencilMask()
{
}

void QStencilMask::setFrontOutputMask(uint mask)
{
    Q_D(QStencilMask);
    if (d->m_frontOutputMask != mask) {
        d->m_frontOutputMask = mask;
        Q_EMIT frontOutputMaskChanged(mask);
    }
}

void QStencilMask::setBackOutputMask(uint mask)
{
    Q_D(QStencilMask);
    if (d->m_backOutputMask != mask) {
        d->m_backOutputMask = mask;
        Q_EMIT backOutputMaskChanged(mask);
    }
}

uint QStencilMask::frontOutputMask() const
{
    Q_D(const QStencilMask);
    return d->m_frontOutputMask;
}

uint QStencilMask::backOutputMask() const
{
    Q_D(const QStencilMask);
    return d->m_backOutputMask;
}

Qt3DCore::QNodeCreatedChangeBasePtr QStencilMask::createNodeCreationChange() const
{
    auto creationChange = QRenderStateCreatedChangePtr<QStencilMaskData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QStencilMask);
    data.frontOutputMask = d->m_frontOutputMask;
    data.backOutputMask = d->m_backOutputMask;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
