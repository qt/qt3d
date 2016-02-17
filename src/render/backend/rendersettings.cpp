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

#include "rendersettings_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/private/abstractrenderer_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

RenderSettings::RenderSettings()
    : BackendNode()
{
}

void RenderSettings::updateFromPeer(Qt3DCore::QNode *peer)
{
    QRenderSettings *settings = static_cast<QRenderSettings *>(peer);
    m_pickMethod = settings->pickMethod();
    m_pickResultMode = settings->pickResultMode();
    m_activeFrameGraph = settings->activeFrameGraph()->id();
}

void RenderSettings::cleanup()
{
    m_pickMethod = QRenderSettings::BoundingVolumePicking;
    m_pickResultMode = QRenderSettings::NearestPick;
    m_activeFrameGraph = Qt3DCore::QNodeId();
}

void RenderSettings::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("pickMethod"))
            m_pickMethod = propertyChange->value().value<QRenderSettings::PickMethod>();
        else if (propertyChange->propertyName() == QByteArrayLiteral("pickResult"))
            m_pickResultMode = propertyChange->value().value<QRenderSettings::PickResultMode>();
        else if (propertyChange->propertyName() == QByteArrayLiteral("activeFrameGraph"))
            m_activeFrameGraph = propertyChange->value().value<QNodePtr>()->id();
        markDirty(AbstractRenderer::AllDirty);
    }
}

RenderSettingsFunctor::RenderSettingsFunctor(AbstractRenderer *renderer)
    : m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RenderSettingsFunctor::create(Qt3DCore::QNode *frontend) const
{
    QRenderSettings *settingsFrontend = static_cast<QRenderSettings *>(frontend);
    if (m_renderer->settings() != Q_NULLPTR) {
        qWarning() << "Renderer settings already exists";
        return Q_NULLPTR;
    }

    RenderSettings *settings = new RenderSettings;
    settings->setPeer(settingsFrontend);
    settings->setRenderer(m_renderer);
    m_renderer->setSettings(settings);
    return settings;
}

Qt3DCore::QBackendNode *RenderSettingsFunctor::get(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    return Q_NULLPTR;
}

void RenderSettingsFunctor::destroy(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    // Deletes the old settings object
    m_renderer->setSettings(Q_NULLPTR);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
