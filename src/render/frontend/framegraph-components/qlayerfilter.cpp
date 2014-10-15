/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlayerfilter.h"
#include "qlayerfilter_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QLayerFilterPrivate::QLayerFilterPrivate(QLayerFilter *qq)
    : QFrameGraphItemPrivate(qq)
{
}

void QLayerFilterPrivate::copy(const QNodePrivate *ref)
{
    QFrameGraphItemPrivate::copy(ref);
    const QLayerFilterPrivate *layer = static_cast<const QLayerFilterPrivate *>(ref);
    m_layers = layer->m_layers;
}

QLayerFilter::QLayerFilter(QNode *parent)
    : QFrameGraphItem(*new QLayerFilterPrivate(this), parent)
{
}

QLayerFilter::QLayerFilter(QLayerFilterPrivate &dd, QNode *parent)
    : QFrameGraphItem(dd, parent)
{
}

QLayerFilter *QLayerFilter::doClone() const
{
    Q_D(const QLayerFilter);
    QLayerFilter *filter = new QLayerFilter();

    filter->d_func()->copy(d_func());

    Q_FOREACH (QFrameGraphItem *fgChild, d->m_fgChildren)
        filter->appendFrameGraphItem(qobject_cast<QFrameGraphItem *>(QNodePrivate::get(fgChild)->clone()));

    return filter;
}

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
