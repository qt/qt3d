// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_FRAMECLEANUPJOB_H
#define QT3DRENDER_RENDER_FRAMECLEANUPJOB_H

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
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class NodeManagers;
class Entity;

class Q_3DRENDERSHARED_PRIVATE_EXPORT FrameCleanupJob : public Qt3DCore::QAspectJob
{
public:
    explicit FrameCleanupJob();
    ~FrameCleanupJob();

    void setRoot(Entity *root);
    void setManagers(NodeManagers *managers);

protected:
    void run() final;

private:
    NodeManagers *m_managers;
    Entity *m_root;
    void updateBoundingVolumesDebug(Entity *node);
};

typedef QSharedPointer<FrameCleanupJob> FrameCleanupJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FRAMECLEANUPJOB_H
