/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_RHI_RHIRESOURCEMANAGERS_P_H
#define QT3DRENDER_RENDER_RHI_RHIRESOURCEMANAGERS_P_H

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

#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>
#include <texture_p.h>
#include <rhibuffer_p.h>
#include <rhishader_p.h>
#include <rhigraphicspipeline_p.h>
#include <Qt3DRender/private/apishadermanager_p.h>
#include <Qt3DRender/private/renderstateset_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace Rhi {

class Q_AUTOTEST_EXPORT RHIBufferManager
    : public Qt3DCore::QResourceManager<RHIBuffer, Qt3DCore::QNodeId, Qt3DCore::NonLockingPolicy>
{
};

class Q_AUTOTEST_EXPORT RHITextureManager
    : public Qt3DCore::QResourceManager<RHITexture, Qt3DCore::QNodeId, Qt3DCore::NonLockingPolicy>
{
public:
    QHash<RHITexture *, Qt3DCore::QNodeId> texNodeIdForRHITexture;
};

class Q_AUTOTEST_EXPORT RHIShaderManager : public APIShaderManager<RHIShader>
{
public:
    explicit RHIShaderManager() : APIShaderManager<RHIShader>() { }
};

// Geometry | Shader | RenderStateMask
struct GraphicsPipelineIdentifier
{
    HGeometry geometry;
    Qt3DCore::QNodeId shader;
    int renderViewIndex;
};

class Q_AUTOTEST_EXPORT RHIGraphicsPipelineManager
    : public Qt3DCore::QResourceManager<RHIGraphicsPipeline, GraphicsPipelineIdentifier,
                                        Qt3DCore::NonLockingPolicy>
{
public:
    RHIGraphicsPipelineManager() { }
};

class Q_AUTOTEST_EXPORT RHIResourceManagers
{
public:
    RHIResourceManagers();
    ~RHIResourceManagers();

    inline RHIShaderManager *rhiShaderManager() const noexcept { return m_rhiShaderManager; }
    inline RHITextureManager *rhiTextureManager() const noexcept { return m_rhiTextureManager; }
    inline RHIBufferManager *rhiBufferManager() const noexcept { return m_rhiBufferManager; }
    inline RHIGraphicsPipelineManager *rhiGraphicsPipelineManager() const noexcept
    {
        return m_rhiGraphicsPipelineManager;
    }

    void releaseAllResources();

private:
    RHIBufferManager *m_rhiBufferManager;
    RHIShaderManager *m_rhiShaderManager;
    RHITextureManager *m_rhiTextureManager;
    RHIGraphicsPipelineManager *m_rhiGraphicsPipelineManager;
};

inline uint qHash(const GraphicsPipelineIdentifier &key, uint seed)
{
    const QPair<HGeometry, Qt3DCore::QNodeId> p = { key.geometry, key.shader };
    using QT_PREPEND_NAMESPACE(qHash);
    return qHash(p, seed) + qHash(key.renderViewIndex, seed);
}

inline bool operator==(const GraphicsPipelineIdentifier &a, const GraphicsPipelineIdentifier &b)
{
    return a.geometry == b.geometry && a.shader == b.shader
            && a.renderViewIndex == b.renderViewIndex;
}

} // Rhi

} // Render

} // Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIGraphicsPipeline, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHITexture, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIBuffer, Q_REQUIRES_CLEANUP)
Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Rhi::RHIShader, Q_REQUIRES_CLEANUP)
QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RHIRESOURCEMANAGERS_P_H
