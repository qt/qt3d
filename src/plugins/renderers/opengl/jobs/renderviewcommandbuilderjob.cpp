/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "renderviewcommandbuilderjob_p.h"
#include <Qt3DRender/private/job_common_p.h>
#include <renderview_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

namespace {
int renderViewInstanceCounter = 0;
} // anonymous

class RenderViewCommandBuilderJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    RenderViewCommandBuilderJobPrivate(RenderViewCommandBuilderJob *q) : q_ptr(q) { }
    ~RenderViewCommandBuilderJobPrivate() override = default;

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    RenderViewCommandBuilderJob *q_ptr;
    Q_DECLARE_PUBLIC(RenderViewCommandBuilderJob)
};

bool RenderViewCommandBuilderJobPrivate::isRequired() const
{
    return q_ptr->m_renderView && !q_ptr->m_renderView->noDraw() && q_ptr->m_count > 0;
}

void RenderViewCommandBuilderJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    Q_UNUSED(manager)
    renderViewInstanceCounter = 0;
}

RenderViewCommandBuilderJob::RenderViewCommandBuilderJob()
    : Qt3DCore::QAspectJob(*new RenderViewCommandBuilderJobPrivate(this))
    , m_offset(0)
    , m_count(0)
    , m_renderView(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::RenderViewCommandBuilder, renderViewInstanceCounter++)
}

void RenderViewCommandBuilderJob::run()
{
    const bool isDraw = !m_renderView->isCompute();
    if (isDraw)
        m_commandData = m_renderView->buildDrawRenderCommands(m_entities, m_offset, m_count);
    else
        m_commandData = m_renderView->buildComputeRenderCommands(m_entities, m_offset, m_count);
}


} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE
