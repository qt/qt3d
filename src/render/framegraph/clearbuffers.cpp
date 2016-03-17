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

#include "clearbuffers_p.h"
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

ClearBuffers::ClearBuffers()
    : FrameGraphNode(FrameGraphNode::ClearBuffers)
    , m_type(QClearBuffers::None)
    , m_clearDepthValue(1.f)
    , m_clearStencilValue(0)
{
}

void ClearBuffers::updateFromPeer(Qt3DCore::QNode *peer)
{
    QClearBuffers *clearBuffers = static_cast<QClearBuffers *>(peer);
    m_type = clearBuffers->buffers();
    m_clearColor = clearBuffers->clearColor();
    m_clearDepthValue = clearBuffers->clearDepthValue();
    m_clearStencilValue = clearBuffers->clearStencilValue();
    setEnabled(clearBuffers->isEnabled());
}

void ClearBuffers::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("buffers"))
            m_type = static_cast<QClearBuffers::BufferType>(propertyChange->value().toInt());
        else if (propertyChange->propertyName() == QByteArrayLiteral("clearColor"))
            m_clearColor = propertyChange->value().value<QColor>();
        else if (propertyChange->propertyName() == QByteArrayLiteral("clearDepthValue"))
            m_clearDepthValue = propertyChange->value().toFloat();
        else if (propertyChange->propertyName() == QByteArrayLiteral("clearStencilValue"))
            m_clearStencilValue = propertyChange->value().toInt();
        else if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
            setEnabled(propertyChange->value().toBool());
        markDirty(AbstractRenderer::AllDirty);
    }
}

QClearBuffers::BufferType ClearBuffers::type() const
{
    return m_type;
}

QColor ClearBuffers::clearColor() const
{
    return m_clearColor;
}

float ClearBuffers::clearDepthValue() const
{
    return m_clearDepthValue;
}

int ClearBuffers::clearStencilValue() const
{
    return m_clearStencilValue;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
