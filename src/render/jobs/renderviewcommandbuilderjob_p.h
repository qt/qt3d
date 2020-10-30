/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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
