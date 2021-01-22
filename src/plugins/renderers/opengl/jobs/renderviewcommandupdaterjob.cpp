/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire
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

#include "renderviewcommandupdaterjob_p.h"
#include <Qt3DRender/private/job_common_p.h>
#include <renderer_p.h>
#include <renderview_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

namespace {
int renderViewInstanceCounter = 0;
} // anonymous

class RenderViewCommandUpdaterJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    RenderViewCommandUpdaterJobPrivate(RenderViewCommandUpdaterJob *q) : q_ptr(q) { }
    ~RenderViewCommandUpdaterJobPrivate() override = default;

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    RenderViewCommandUpdaterJob *q_ptr;
    Q_DECLARE_PUBLIC(RenderViewCommandUpdaterJob)
};

bool RenderViewCommandUpdaterJobPrivate::isRequired() const
{
    Q_Q(const RenderViewCommandUpdaterJob);
    return q->m_renderView && !q->m_renderView->noDraw() && q->m_renderablesSubView.count > 0;
}

void RenderViewCommandUpdaterJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    Q_UNUSED(manager)

    // reset to 0 after every frame, stops the number growing indefinitely
    renderViewInstanceCounter = 0;
}

RenderViewCommandUpdaterJob::RenderViewCommandUpdaterJob()
    : Qt3DCore::QAspectJob(*new RenderViewCommandUpdaterJobPrivate(this))
    , m_renderView(nullptr)
    , m_renderer(nullptr)
    , m_renderablesSubView()
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::RenderCommandUpdater, renderViewInstanceCounter++)
}

void RenderViewCommandUpdaterJob::run()
{
    // Build RenderCommand should perform the culling as we have no way to determine
    // if a child has a mesh in the view frustum while its parent isn't contained in it.
    if (!m_renderView->noDraw()) {
        if (m_renderablesSubView.count == 0)
            return;
        // Update Render Commands (Uniform Change, Depth Change)
        m_renderView->updateRenderCommand(m_renderablesSubView);
    }
}

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE
