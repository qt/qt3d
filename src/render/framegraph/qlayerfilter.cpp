/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qlayerfilter.h"
#include "qlayerfilter_p.h"
#include <Qt3DCore/qnodepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QLayerFilterPrivate::QLayerFilterPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
    \class Qt3DRender::QLayerFilter
    \inmodule Qt3DRender
    \since 5.5
    \brief The QLayerFilter class provides ...
*/

/*!
    \qmltype LayerFilter
    \instantiates Qt3DRender::QLayerFilter
    \inherits FrameGraphNode
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief For ...
*/

/*! \fn void Qt3DRender::QLayerFilter::copy(const Qt3DCore::QNode *ref)
  Copies the \a ref instance into this one.
 */
void QLayerFilter::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QLayerFilter *layer = static_cast<const QLayerFilter*>(ref);
    d_func()->m_layers = layer->d_func()->m_layers;
}


/*! \fn Qt3DRender::QLayerFilter::QLayerFilter(Qt3DCore::QNode *parent)
  Constructs a new QLayerFilter with the specified \a parent.
 */
QLayerFilter::QLayerFilter(QNode *parent)
    : QFrameGraphNode(*new QLayerFilterPrivate, parent)
{
}

/*! \internal */
QLayerFilter::QLayerFilter(QLayerFilterPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

/*!
  \property Qt3DRender::QLayerFilter::layers

 */

/*!
  \qmlproperty stringlist Qt3D.Render::LayerFilter::layers

*/

void QLayerFilter::setLayers(const QStringList &layers)
{
    Q_D(QLayerFilter);
    if (d->m_layers != layers) {
        d->m_layers = layers;
        emit layersChanged(layers);
    }
}

QStringList QLayerFilter::layers() const
{
    Q_D(const QLayerFilter);
    return d->m_layers;
}

Qt3DCore::QNodeCreatedChangeBasePtr QLayerFilter::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<QLayerFilterData>::create(this);
    auto &data = creationChange->data;
    Q_D(const QLayerFilter);
    data.layers = d->m_layers;
    return creationChange;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
