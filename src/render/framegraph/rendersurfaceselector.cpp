/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#include "rendersurfaceselector_p.h"
#include <Qt3DRender/qrendersurfaceselector.h>

#include <Qt3DCore/qscenepropertychange.h>

#include <QtGui/qwindow.h>
#include <QtGui/qoffscreensurface.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

RenderSurfaceSelector::RenderSurfaceSelector()
    : FrameGraphNode(FrameGraphNode::Surface)
    , m_surface(Q_NULLPTR)
{
}

void RenderSurfaceSelector::updateFromPeer(Qt3DCore::QNode *peer)
{
    QRenderSurfaceSelector *selector = static_cast<QRenderSurfaceSelector *>(peer);
    m_surface = selector->surface();
    setEnabled(selector->isEnabled());
}

void RenderSurfaceSelector::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    qCDebug(Render::Framegraph) << Q_FUNC_INFO;
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("surface"))
            m_surface = propertyChange->value().value<QSurface *>();
        else if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
            setEnabled(propertyChange->value().toBool());
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
