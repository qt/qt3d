// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "environmentlight_p.h"
#include "qenvironmentlight.h"
#include "qenvironmentlight_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

QNodeId EnvironmentLight::shaderData() const
{
    return m_shaderDataId;
}

void EnvironmentLight::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QEnvironmentLight *node = qobject_cast<const QEnvironmentLight *>(frontEnd);
    if (!node)
        return;

    if (firstTime) {
        QEnvironmentLightPrivate *d = static_cast<QEnvironmentLightPrivate *>(QEnvironmentLightPrivate::get(const_cast<Qt3DCore::QNode *>(frontEnd)));
        m_shaderDataId = d->m_shaderData ? d->m_shaderData->id() : QNodeId{};
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
