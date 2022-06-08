// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "lightgatherer_p.h"
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

LightGatherer::LightGatherer()
    : Qt3DCore::QAspectJob()
    , m_manager(nullptr)
    , m_environmentLight(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LightGathering, 0)
}

void LightGatherer::run()
{
    m_lights.clear();
    m_environmentLight = nullptr;

    const std::vector<HEntity> &handles = m_manager->activeHandles();
    size_t envLightCount = 0;

    for (const HEntity &handle : handles) {
        Entity *node = m_manager->data(handle);
        std::vector<Light *> lights = node->renderComponents<Light>();
        if (!lights.empty())
            m_lights.push_back(LightSource(node, std::move(lights)));
        const std::vector<EnvironmentLight *> &envLights = node->renderComponents<EnvironmentLight>();
        envLightCount += envLights.size();
        if (!envLights.empty() && !m_environmentLight)
            m_environmentLight = envLights.front();
    }

    if (envLightCount > 1)
        qWarning() << "More than one environment light found, extra instances are ignored";
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
