/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlayerfilter.h"
#include "qlayerfilter_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QLayerFilterPrivate
    \internal
*/
QLayerFilterPrivate::QLayerFilterPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
    \class QLayerFilter
    \inmodule Qt3DRenderer
    \since 5.5
    \brief The QLayerFilter class provides ...
*/

/*!
    \qmltype LayerFilter
    \instantiates Qt3D::QLayerFilter
    \inherits FrameGraphNode
    \inqmlmodule Qt3D.Renderer
    \since 5.5
    \brief For ...
*/

/*! \fn void Qt3D::QLayerFilter::copy(const QNode *ref)
  Copies the \a ref instance into this one.
 */
void QLayerFilter::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QLayerFilter *layer = static_cast<const QLayerFilter*>(ref);
    d_func()->m_layers = layer->d_func()->m_layers;
}


/*! \fn Qt3D::QLayerFilter::QLayerFilter(QNode *parent)
  Constructs a new QLayerFilter with the specified \a parent.
 */
QLayerFilter::QLayerFilter(QNode *parent)
    : QFrameGraphNode(*new QLayerFilterPrivate, parent)
{
}

QLayerFilter::~QLayerFilter()
{
    QNode::cleanup();
}

/*! \internal */
QLayerFilter::QLayerFilter(QLayerFilterPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

/*!
  \property Qt3D::QLayerFilter::layers

 */

/*!
  \qmlproperty stringlist Qt3D.Renderer::LayerFilter::layers

*/

void QLayerFilter::setLayers(const QStringList &layers)
{
    Q_D(QLayerFilter);
    if (d->m_layers != layers) {
        d->m_layers = layers;
        emit layersChanged();
    }
}

QStringList QLayerFilter::layers() const
{
    Q_D(const QLayerFilter);
    return d->m_layers;
}


} // Qt3D

QT_END_NAMESPACE
