// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_TECHNIQUEMANAGER_H
#define QT3DRENDER_RENDER_TECHNIQUEMANAGER_H

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

#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRender/private/technique_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT TechniqueManager : public Qt3DCore::QResourceManager<
        Technique,
        Qt3DCore::QNodeId,
        Qt3DCore::NonLockingPolicy>
{
public:
    TechniqueManager();

    void addDirtyTechnique(Qt3DCore::QNodeId techniqueId);
    std::vector<Qt3DCore::QNodeId> takeDirtyTechniques();

private:
    std::vector<Qt3DCore::QNodeId> m_dirtyTechniques;
};

} // Render
} // Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Technique, Q_REQUIRES_CLEANUP)

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_TECHNIQUEMANAGER_H
