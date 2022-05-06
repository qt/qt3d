/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
