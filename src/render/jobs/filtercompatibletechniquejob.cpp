// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "filtercompatibletechniquejob_p.h"
#include <Qt3DRender/private/techniquemanager_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

FilterCompatibleTechniqueJob::FilterCompatibleTechniqueJob()
    : m_manager(nullptr)
    , m_renderer(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::FilterCompatibleTechniques, 0)
}

void FilterCompatibleTechniqueJob::setManager(TechniqueManager *manager)
{
    m_manager = manager;
}

TechniqueManager *FilterCompatibleTechniqueJob::manager() const
{
    return m_manager;
}

void FilterCompatibleTechniqueJob::setRenderer(AbstractRenderer *renderer)
{
    m_renderer = renderer;
}

AbstractRenderer *FilterCompatibleTechniqueJob::renderer() const
{
    return m_renderer;
}

void FilterCompatibleTechniqueJob::run()
{
    Q_ASSERT(m_manager != nullptr && m_renderer != nullptr);
    Q_ASSERT(m_renderer->isRunning());

    const std::vector<Qt3DCore::QNodeId> dirtyTechniqueIds = m_manager->takeDirtyTechniques();
    for (const Qt3DCore::QNodeId &techniqueId : dirtyTechniqueIds) {
        Technique *technique = m_manager->lookupResource(techniqueId);
        if (Q_LIKELY(technique != nullptr))
            technique->setCompatibleWithRenderer((*m_renderer->contextInfo() == *technique->graphicsApiFilter()));
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
