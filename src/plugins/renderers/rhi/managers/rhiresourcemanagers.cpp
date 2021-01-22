/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#include "rhiresourcemanagers_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

RHIResourceManagers::RHIResourceManagers()
    : m_rhiBufferManager(new RHIBufferManager()),
      m_rhiShaderManager(new RHIShaderManager()),
      m_rhiTextureManager(new RHITextureManager()),
      m_rhiGraphicsPipelineManager(new RHIGraphicsPipelineManager())
{
}

RHIResourceManagers::~RHIResourceManagers()
{
    delete m_rhiTextureManager;
    delete m_rhiShaderManager;
    delete m_rhiBufferManager;
    delete m_rhiGraphicsPipelineManager;
}

void RHIResourceManagers::releaseAllResources()
{
    auto releaseAll = [](auto *manager) noexcept {
        const auto handles = manager->activeHandles();
        for (const auto &handle : handles) {
            manager->release(handle);
        }
    };

    releaseAll(m_rhiTextureManager);
    releaseAll(m_rhiBufferManager);
    // releaseAll(m_rhiShaderManager);
    releaseAll(m_rhiGraphicsPipelineManager);
}

} // Rhi

} // Render

} // Qt3DRender

QT_END_NAMESPACE
