// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_UPDATELEVELOFDETAILJOB_H
#define QT3DRENDER_RENDER_UPDATELEVELOFDETAILJOB_H

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

#include <Qt3DCore/QAspectJob>
#include <Qt3DCore/QNodeId>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Entity;
class NodeManagers;
class LevelOfDetail;
class FrameGraphNode;
class UpdateLevelOfDetailJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT UpdateLevelOfDetailJob : public Qt3DCore::QAspectJob
{
public:
    UpdateLevelOfDetailJob();
    ~UpdateLevelOfDetailJob();

    void setManagers(NodeManagers *manager);
    void setFrameGraphRoot(FrameGraphNode *frameGraphRoot);
    void setRoot(Entity *root);
    void run() final;

    NodeManagers *managers() const { return m_manager; }
    Entity *root() const { return m_root; }

private:
    Q_DECLARE_PRIVATE(UpdateLevelOfDetailJob)

    NodeManagers *m_manager;
    FrameGraphNode *m_frameGraphRoot;
    Entity *m_root;
    double m_filterValue;
};

typedef QSharedPointer<UpdateLevelOfDetailJob> UpdateLevelOfDetailJobPtr;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_UPDATELEVELOFDETAILJOB_H
