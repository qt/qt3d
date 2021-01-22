/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_RHI_RENDERVIEWCOMMANDUPDATEJOB_P_H
#define QT3DRENDER_RENDER_RHI_RENDERVIEWCOMMANDUPDATEJOB_P_H

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
#include <rendercommand_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class RenderView;
class Renderer;

class Q_AUTOTEST_EXPORT RenderViewCommandUpdaterJob : public Qt3DCore::QAspectJob
{
public:
    RenderViewCommandUpdaterJob();

    inline void setRenderView(RenderView *rv) Q_DECL_NOTHROW { m_renderView = rv; }
    inline void setRenderer(Renderer *renderer) Q_DECL_NOTHROW { m_renderer = renderer; }
    inline void setRenderables(const EntityRenderCommandDataPtr &renderables, int offset,
                               int count) Q_DECL_NOTHROW
    {
        m_offset = offset;
        m_count = count;
        m_renderables = renderables;
    }
    EntityRenderCommandDataPtr renderables() const { return m_renderables; }

    QVector<RenderCommand> &commands() Q_DECL_NOTHROW { return m_commands; }

    void run() final;

private:
    int m_offset;
    int m_count;
    RenderView *m_renderView;
    Renderer *m_renderer;
    EntityRenderCommandDataPtr m_renderables;
    QVector<RenderCommand> m_commands;
};

typedef QSharedPointer<RenderViewCommandUpdaterJob> RenderViewCommandUpdaterJobPtr;

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RENDERVIEWCOMMANDUPDATEJOB_P_H
