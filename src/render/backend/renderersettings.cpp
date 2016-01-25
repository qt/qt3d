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

#include "renderersettings_p.h"

#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/private/abstractrenderer_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

RendererSettings::RendererSettings()
    : QBackendNode()
{
}

void RendererSettings::updateFromPeer(Qt3DCore::QNode *peer)
{
    QRendererSettings *settings = static_cast<QRendererSettings *>(peer);
    m_pickMethod = settings->pickMethod();
    m_pickResultMode = settings->pickResultMode();
}

void RendererSettings::cleanup()
{
    m_pickMethod = QRendererSettings::BoundingVolumePicking;
    m_pickResultMode = QRendererSettings::NearestPick;
}

void RendererSettings::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == NodeUpdated) {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("pickMethod"))
            m_pickMethod = propertyChange->value().value<QRendererSettings::PickMethod>();
        else if (propertyChange->propertyName() == QByteArrayLiteral("pickResult"))
            m_pickResultMode = propertyChange->value().value<QRendererSettings::PickResultMode>();
    }
}


RendererSettingsFunctor::RendererSettingsFunctor(AbstractRenderer *renderer)
    : m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *RendererSettingsFunctor::create(Qt3DCore::QNode *frontend) const
{
    QRendererSettings *settingsFrontend = static_cast<QRendererSettings *>(frontend);
    if (m_renderer->settings() != Q_NULLPTR) {
        qWarning() << "Renderer settings already exists";
        return Q_NULLPTR;
    }

    RendererSettings *settings = new RendererSettings;
    settings->setPeer(settingsFrontend);
    m_renderer->setSettings(settings);
    return settings;
}

Qt3DCore::QBackendNode *RendererSettingsFunctor::get(const Qt3DCore::QNodeId &id) const
{
    Q_UNUSED(id);
    return Q_NULLPTR;
}

void RendererSettingsFunctor::destroy(const Qt3DCore::QNodeId &id) const
{
    Q_UNUSED(id);
    // Deletes the old settings object
    m_renderer->setSettings(Q_NULLPTR);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
