// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERVIEWCOMMANDBUILDERJOB_P_H
#define QT3DRENDER_RENDER_RENDERVIEWCOMMANDBUILDERJOB_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/renderercache_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

template<class RenderView, class RenderCommand>
class RenderViewCommandBuilderJob : public Qt3DCore::QAspectJob
{
public:
    RenderViewCommandBuilderJob()
    {
        SET_JOB_RUN_STAT_TYPE(this, JobTypes::RenderViewCommandBuilder, RenderViewCommandBuilderJob::renderViewInstanceCounter++)
    }

    inline void setRenderView(RenderView *rv) noexcept { m_renderView = rv; }
    inline void setEntities(const Entity **entities, int offset, int count)
    {
        m_offset = offset;
        m_count = count;
        m_entities = entities;
    }
    inline EntityRenderCommandData<RenderCommand> &commandData() { return m_commandData; }

    void run() final
    {
        const bool isDraw = !m_renderView->isCompute();
        if (isDraw)
            m_commandData = m_renderView->buildDrawRenderCommands(m_entities, m_offset, m_count);
        else
            m_commandData = m_renderView->buildComputeRenderCommands(m_entities, m_offset, m_count);
    }

    bool isRequired() override
    {
        return m_renderView && !m_renderView->noDraw() && m_count > 0;
    }

    void postFrame(Qt3DCore::QAspectEngine *engine) override
    {
        Q_UNUSED(engine);
        RenderViewCommandBuilderJob::renderViewInstanceCounter = 0;
    }

private:
    RenderView *m_renderView = nullptr;
    const Entity **m_entities = nullptr;
    EntityRenderCommandData<RenderCommand> m_commandData;
    int m_offset = 0;
    int m_count = 0;
    static int renderViewInstanceCounter;
};

template<class RenderView, class RenderCommand>
int RenderViewCommandBuilderJob<RenderView, RenderCommand>::renderViewInstanceCounter = 0;

template<class RenderView, class RenderCommand>
using RenderViewCommandBuilderJobPtr = QSharedPointer<RenderViewCommandBuilderJob<RenderView, RenderCommand>>;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERVIEWCOMMANDBUILDERJOB_P_H
