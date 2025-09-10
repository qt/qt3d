// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERPASS_H
#define QT3DRENDER_RENDER_RENDERPASS_H

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
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/private/parameterpack_p.h>
#include <Qt3DCore/private/qabstractaspect_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderPass;
class QAbstractShader;
class QFilterKey;
class QRenderState;

namespace Render {

class RenderPassManager;
class RenderState;

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderPass : public BackendNode
{
public:
    RenderPass();
    ~RenderPass();

    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    Qt3DCore::QNodeId shaderProgram() const;
    QList<Qt3DCore::QNodeId> filterKeys() const;
    QList<Qt3DCore::QNodeId> parameters() const;
    QList<Qt3DCore::QNodeId> renderStates() const;

    inline bool hasRenderStates() const { return !m_renderStates.empty(); }

private:
    void appendFilterKey(Qt3DCore::QNodeId filterKeyId);
    void removeFilterKey(Qt3DCore::QNodeId filterKeyId);

    void addRenderState(Qt3DCore::QNodeId renderStateId);
    void removeRenderState(Qt3DCore::QNodeId renderStateId);

    Qt3DCore::QNodeId m_shaderUuid;
    QList<Qt3DCore::QNodeId> m_filterKeyList;
    ParameterPack m_parameterPack;
    QList<Qt3DCore::QNodeId> m_renderStates;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERPASS_H
