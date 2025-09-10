// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_MATERIALPARAMETERGATHERERJOB_P_H
#define QT3DRENDER_RENDER_MATERIALPARAMETERGATHERERJOB_P_H

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
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class NodeManagers;
class TechniqueFilter;
class RenderPassFilter;

// TO be executed for each FrameGraph branch with a given RenderPassFilter/TechniqueFilter
class MaterialParameterGathererJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT MaterialParameterGathererJob : public Qt3DCore::QAspectJob
{
public:
    MaterialParameterGathererJob();

    inline void setNodeManagers(NodeManagers *manager) noexcept { m_manager = manager; }
    inline void setTechniqueFilter(TechniqueFilter *techniqueFilter) noexcept { m_techniqueFilter = techniqueFilter; }
    inline void setRenderPassFilter(RenderPassFilter *renderPassFilter) noexcept { m_renderPassFilter = renderPassFilter; }
    inline const MaterialParameterGathererData &materialToPassAndParameter() noexcept { return m_parameters; }
    inline void setHandles(std::vector<HMaterial> &&handles) noexcept { m_handles = std::move(handles); }
    inline void setHandles(const std::vector<HMaterial> &handles) noexcept { m_handles = handles; }

    inline TechniqueFilter *techniqueFilter() const noexcept { return m_techniqueFilter; }
    inline RenderPassFilter *renderPassFilter() const noexcept { return m_renderPassFilter; }

    void run() final;

private:
    NodeManagers *m_manager;
    TechniqueFilter *m_techniqueFilter;
    RenderPassFilter *m_renderPassFilter;

    // Material id to array of RenderPasse with parameters
    MaterialParameterGathererData m_parameters;
    std::vector<HMaterial> m_handles;

    Q_DECLARE_PRIVATE(MaterialParameterGathererJob)
};

typedef QSharedPointer<MaterialParameterGathererJob> MaterialParameterGathererJobPtr;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_MATERIALPARAMETERGATHERERJOB_P_H
