// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "techniquemanager_p.h"
#include <Qt3DCore/private/vector_helper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

TechniqueManager::TechniqueManager()
{
}

// AspectThread
void TechniqueManager::addDirtyTechnique(Qt3DCore::QNodeId techniqueId)
{
    if (!Qt3DCore::contains(m_dirtyTechniques, techniqueId))
        m_dirtyTechniques.push_back(techniqueId);
}

// AspectThread
std::vector<Qt3DCore::QNodeId> TechniqueManager::takeDirtyTechniques()
{
    return Qt3DCore::moveAndClear(m_dirtyTechniques);
}

} // Render
} // Qt3DRender

QT_END_NAMESPACE
