// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_COMPUTEFILTEREDBOUNDINGVOLUMEJOB_H
#define QT3DRENDER_RENDER_COMPUTEFILTEREDBOUNDINGVOLUMEJOB_H

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
#include <private/qt3drender_global_p.h>

#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QAspectManager;
}

namespace Qt3DRender {
namespace Render {

class Entity;
class NodeManagers;
class Sphere;
class ComputeFilteredBoundingVolumeJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT ComputeFilteredBoundingVolumeJob : public Qt3DCore::QAspectJob
{
public:
    ComputeFilteredBoundingVolumeJob();

    void setRoot(Entity *root);
    void setManagers(NodeManagers *manager);
    void ignoreSubTree(Entity *node);
    void run() override;

protected:
    virtual void finished(Qt3DCore::QAspectManager *aspectManager, const Qt3DRender::Render::Sphere &sphere);    // called in main thread

private:
    Q_DECLARE_PRIVATE(ComputeFilteredBoundingVolumeJob)
    Entity *m_root;
    Entity *m_ignoreSubTree;
    NodeManagers *m_manager;
};

typedef QSharedPointer<ComputeFilteredBoundingVolumeJob> ComputeFilteredBoundingVolumeJobPtr;

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_COMPUTEFILTEREDBOUNDINGVOLUMEJOB_H
