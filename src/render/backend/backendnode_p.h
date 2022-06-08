// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_BACKENDNODE_H
#define QT3DRENDER_RENDER_BACKENDNODE_H

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

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DRender/private/abstractrenderer_p.h>
#include <private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QBackendNodePrivate;

}

namespace Qt3DRender {

namespace Render {

class RenderBackendResourceAccessor;

class Q_3DRENDERSHARED_PRIVATE_EXPORT BackendNode : public Qt3DCore::QBackendNode
{
public:
    BackendNode(Qt3DCore::QBackendNode::Mode mode = ReadOnly);
    ~BackendNode();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    void setRenderer(AbstractRenderer *renderer);
    AbstractRenderer *renderer() const;

    QSharedPointer<RenderBackendResourceAccessor> resourceAccessor() const;

protected:
    explicit BackendNode(Qt3DCore::QBackendNodePrivate &dd);
    void markDirty(AbstractRenderer::BackendNodeDirtySet changes);
    AbstractRenderer *m_renderer;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_BACKENDNODE_H
