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

#include "rendertargetselectornode_p.h"
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/rendereraspect.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DRenderer/qrendertargetselector.h>
#include <Qt3DRenderer/qrendertarget.h>
#include <Qt3DCore/qscenepropertychange.h>
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderTargetSelector::RenderTargetSelector() :
    FrameGraphNode(FrameGraphNodeType::RenderTarget)
{
}

void RenderTargetSelector::updateFromPeer(QNode *peer)
{
    QRenderTargetSelector *selector = static_cast<QRenderTargetSelector *>(peer);
    m_renderTargetUuid = QUuid();
    if (selector->target() != Q_NULLPTR)
        m_renderTargetUuid = selector->target()->uuid();
}

void RenderTargetSelector::sceneChangeEvent(const QSceneChangePtr &e)
{
    qCDebug(Render::Framegraph) << Q_FUNC_INFO;
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("target"))
            m_renderTargetUuid = propertyChange->value().toUuid();
    }
}

QUuid RenderTargetSelector::renderTargetUuid() const
{
    return m_renderTargetUuid;
}

} // Render
} // Qt3D

QT_END_NAMESPACE
