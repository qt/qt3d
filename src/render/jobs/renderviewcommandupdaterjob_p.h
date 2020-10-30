/****************************************************************************
**
** Copyright (C) 2016 Paul Lemire
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

#ifndef QT3DRENDER_RENDER_RENDERVIEWCOMMANDUPDATEJOB_P_H
#define QT3DRENDER_RENDER_RENDERVIEWCOMMANDUPDATEJOB_P_H

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
class RenderViewCommandUpdaterJob : public Qt3DCore::QAspectJob
{
public:
    RenderViewCommandUpdaterJob()
        : Qt3DCore::QAspectJob()
    {
        SET_JOB_RUN_STAT_TYPE(this, JobTypes::RenderCommandUpdater, renderViewInstanceCounter++)
    }

    ~RenderViewCommandUpdaterJob()
    {
    }

    inline void setRenderView(RenderView *rv) noexcept { m_renderView = rv; }
    inline void setRenderablesSubView(const EntityRenderCommandDataSubView<RenderCommand> &renderablesSubView) noexcept
    {
        m_renderablesSubView = renderablesSubView;
    }
    EntityRenderCommandDataSubView<RenderCommand> renderablesSubView() const { return m_renderablesSubView; }

    void run() final
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

    void postFrame(Qt3DCore::QAspectEngine *aspectEngine) override
    {
        Q_UNUSED(aspectEngine);
        // reset to 0 after every frame, stops the number growing indefinitely
        renderViewInstanceCounter = 0;
    }

    bool isRequired() override
    {
        return m_renderView && !m_renderView->noDraw() && m_renderablesSubView.count > 0;
    }

private:
    RenderView *m_renderView = nullptr;
    EntityRenderCommandDataSubView<RenderCommand> m_renderablesSubView;
    static int renderViewInstanceCounter;
};

template<class RenderView, class RenderCommand>
int RenderViewCommandUpdaterJob<RenderView, RenderCommand>::renderViewInstanceCounter = 0;

template<class RenderView, class RenderCommand>
using RenderViewCommandUpdaterJobPtr = QSharedPointer<RenderViewCommandUpdaterJob<RenderView, RenderCommand>>;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERVIEWCOMMANDUPDATEJOB_P_H
