// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_UPDATESKINNINGPALETTEJOB_P_H
#define QT3DRENDER_RENDER_UPDATESKINNINGPALETTEJOB_P_H

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

#include <QtCore/qsharedpointer.h>

#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class NodeManagers;

class Q_3DRENDERSHARED_PRIVATE_EXPORT UpdateSkinningPaletteJob : public Qt3DCore::QAspectJob
{
public:
    explicit UpdateSkinningPaletteJob();
    ~UpdateSkinningPaletteJob();

    void setRoot(Entity *root) { m_root = root; }
    void setManagers(NodeManagers *nodeManagers) { m_nodeManagers = nodeManagers; }

    void setDirtyJoints(const QList<HJoint> dirtyJoints) { m_dirtyJoints = dirtyJoints; }
    void clearDirtyJoints() { m_dirtyJoints.clear(); }

protected:
    void run() override;
    NodeManagers *m_nodeManagers;
    Entity *m_root;
    QList<HJoint> m_dirtyJoints;
};

typedef QSharedPointer<UpdateSkinningPaletteJob> UpdateSkinningPaletteJobPtr;

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_UPDATESKINNINGPALETTEJOB_P_H
