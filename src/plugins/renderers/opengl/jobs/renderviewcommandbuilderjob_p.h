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

#ifndef QT3DRENDER_RENDER_OPENGL_RENDERVIEWCOMMANDBUILDERJOB_P_H
#define QT3DRENDER_RENDER_OPENGL_RENDERVIEWCOMMANDBUILDERJOB_P_H

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

namespace OpenGL {

class RenderViewCommandBuilderJobPrivate;

class Q_AUTOTEST_EXPORT RenderViewCommandBuilderJob : public Qt3DCore::QAspectJob
{
public:
    RenderViewCommandBuilderJob();

    inline void setRenderView(RenderView *rv) Q_DECL_NOTHROW { m_renderView = rv; }
    inline void setEntities(const QVector<Entity *> &entities, int offset, int count)
    {
        m_offset = offset;
        m_count = count;
        m_entities = entities;
    }
    inline EntityRenderCommandData &commandData() { return m_commandData; }

    void run() final;

private:
    int m_offset;
    int m_count;
    RenderView *m_renderView;
    QVector<Entity *> m_entities;
    EntityRenderCommandData m_commandData;

    Q_DECLARE_PRIVATE(RenderViewCommandBuilderJob)
};

typedef QSharedPointer<RenderViewCommandBuilderJob> RenderViewCommandBuilderJobPtr;

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_RENDERVIEWCOMMANDBUILDERJOB_P_H
