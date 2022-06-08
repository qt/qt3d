// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERTARGET_H
#define QT3DRENDER_RENDER_RENDERTARGET_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <QList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderTarget;
class QRenderTargetOutput;

namespace Render {

class RenderTargetManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderTarget : public BackendNode
{
public:
    RenderTarget();
    void cleanup();

    void appendRenderOutput(Qt3DCore::QNodeId outputId);
    void removeRenderOutput(Qt3DCore::QNodeId outputId);

    QList<Qt3DCore::QNodeId> renderOutputs() const;

    bool isDirty() const;
    void unsetDirty();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QList<Qt3DCore::QNodeId> m_renderOutputs;
    bool m_dirty;
};

class Q_AUTOTEST_EXPORT RenderTargetFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit RenderTargetFunctor(AbstractRenderer *renderer,
                                 RenderTargetManager *manager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    AbstractRenderer *m_renderer;
    RenderTargetManager *m_renderTargetManager;
};


} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERTARGET_H
