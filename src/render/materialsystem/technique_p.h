// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_TECHNIQUE_H
#define QT3DRENDER_RENDER_TECHNIQUE_H

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
#include <Qt3DRender/private/parameterpack_p.h>
#include <Qt3DRender/private/qgraphicsapifilter_p.h>
#include <Qt3DRender/qfilterkey.h>
#include <QList>
#include <QStringList>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTechnique;
class QParameter;
class QGraphicsApiFilter;
class QFilterKey;
class QRenderPass;

namespace Render {

class TechniqueManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Technique : public BackendNode
{
public:
    Technique();
    ~Technique();
    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    QList<Qt3DCore::QNodeId> parameters() const;

    void appendRenderPass(Qt3DCore::QNodeId renderPassId);
    void removeRenderPass(Qt3DCore::QNodeId renderPassId);

    void appendFilterKey(Qt3DCore::QNodeId criterionId);
    void removeFilterKey(Qt3DCore::QNodeId criterionId);

    QList<Qt3DCore::QNodeId> filterKeys() const;
    QList<Qt3DCore::QNodeId> renderPasses() const;
    const GraphicsApiFilterData *graphicsApiFilter() const;

    bool isCompatibleWithRenderer() const;
    void setCompatibleWithRenderer(bool compatible);

    bool isCompatibleWithFilters(const Qt3DCore::QNodeIdVector &filterKeyIds);

    void setNodeManager(NodeManagers *nodeManager);
    NodeManagers *nodeManager() const;

private:

    GraphicsApiFilterData m_graphicsApiFilterData;
    ParameterPack m_parameterPack;
    QList<Qt3DCore::QNodeId> m_filterKeyList;
    QList<Qt3DCore::QNodeId> m_renderPasses;
    bool m_isCompatibleWithRenderer;
    NodeManagers *m_nodeManager;
};

class TechniqueFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit TechniqueFunctor(AbstractRenderer *renderer, NodeManagers *manager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;
private:
    NodeManagers *m_manager;
    AbstractRenderer *m_renderer;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_TECHNIQUE_H
