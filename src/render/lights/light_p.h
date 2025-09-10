// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LIGHT_P_H
#define QT3DRENDER_RENDER_LIGHT_P_H

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

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class NodeManagers;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Light : public BackendNode
{
public:
    Qt3DCore::QNodeId shaderData() const;

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    Qt3DCore::QNodeId m_shaderDataId;
};

class RenderLightFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit RenderLightFunctor(AbstractRenderer *renderer, NodeManagers *managers);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final;
    void destroy(Qt3DCore::QNodeId id) const final;

private:
    NodeManagers *m_managers;
    AbstractRenderer *m_renderer;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Render::Light*) // LCOV_EXCL_LINE

#endif // QT3DRENDER_RENDER_LIGHT_P_H
