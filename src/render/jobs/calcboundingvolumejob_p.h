// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_CALCBOUNDINGVOLUMEJOB_H
#define QT3DRENDER_RENDER_CALCBOUNDINGVOLUMEJOB_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/private/calcboundingvolumejob_p.h>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QAbstractFrontEndNodeManager;
}

namespace Qt3DRender {
namespace Render {

class NodeManagers;
class Entity;
class Geometry;

class Q_3DRENDERSHARED_PRIVATE_EXPORT CalculateBoundingVolumeJob : public Qt3DCore::QAspectJob, public Qt3DCore::BoundingVolumeJobProcessor
{
public:
    explicit CalculateBoundingVolumeJob();

    void setRoot(Entity *node);
    void setManagers(NodeManagers *manager);
    void setFrontEndNodeManager(Qt3DCore::QAbstractFrontEndNodeManager *manager);
    void run() override;

    void postFrame(Qt3DCore::QAspectEngine *aspectEngine) override;

private:
    void process(const Qt3DCore::BoundingVolumeComputeResult &result, bool computedResult) override;

    NodeManagers *m_manager;
    Entity *m_node;
    Qt3DCore::QAbstractFrontEndNodeManager *m_frontEndNodeManager;
    std::vector<Geometry *> m_updatedGeometries;
};

typedef QSharedPointer<CalculateBoundingVolumeJob> CalculateBoundingVolumeJobPtr;

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_CALCBOUNDINGVOLUMEJOB_H
