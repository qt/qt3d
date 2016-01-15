/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qrendersurfaceselector.h"
#include "qrendersurfaceselector_p.h"

#include <QtGui/qwindow.h>
#include <QtGui/qoffscreensurface.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QRenderSurfaceSelectorPrivate::QRenderSurfaceSelectorPrivate()
    : Qt3DRender::QFrameGraphNodePrivate()
{
}

QRenderSurfaceSelector::QRenderSurfaceSelector(Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(*new QRenderSurfaceSelectorPrivate, parent)
{
}

QRenderSurfaceSelector::QRenderSurfaceSelector(QRenderSurfaceSelectorPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DRender::QFrameGraphNode(dd, parent)
{
}

QRenderSurfaceSelector::~QRenderSurfaceSelector()
{
    QNode::cleanup();
}

QSurface *QRenderSurfaceSelector::surface() const
{
    Q_D(const QRenderSurfaceSelector);
    return d->m_surface;
}

void QRenderSurfaceSelector::setSurface(QSurface *surface)
{
    Q_D(QRenderSurfaceSelector);
    if (d->m_surface == surface)
        return;

    d->m_surface = surface;
    emit surfaceChanged(surface);
}

void QRenderSurfaceSelector::copy(const QNode *ref)
{
    QFrameGraphNode::copy(ref);
    const QRenderSurfaceSelector *object = static_cast<const QRenderSurfaceSelector *>(ref);

    d_func()->m_surface = object->d_func()->m_surface;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
