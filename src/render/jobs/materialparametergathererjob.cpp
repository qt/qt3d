// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "materialparametergathererjob_p.h"
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/renderpassfilternode_p.h>
#include <Qt3DRender/private/techniquefilternode_p.h>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace {

int materialParameterGathererCounter = 0;
const int likelyNumberOfParameters = 24;

} // anonymous

class MaterialParameterGathererJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    MaterialParameterGathererJobPrivate(MaterialParameterGathererJob *q) : q_ptr(q) { }
    ~MaterialParameterGathererJobPrivate() override = default;

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    MaterialParameterGathererJob *q_ptr;
    Q_DECLARE_PUBLIC(MaterialParameterGathererJob)
};

bool MaterialParameterGathererJobPrivate::isRequired() const
{
    return !q_ptr->m_handles.empty();
}

void MaterialParameterGathererJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    Q_UNUSED(manager);
    materialParameterGathererCounter = 0;
}

MaterialParameterGathererJob::MaterialParameterGathererJob()
    : Qt3DCore::QAspectJob(*new MaterialParameterGathererJobPrivate(this))
    , m_manager(nullptr)
    , m_techniqueFilter(nullptr)
    , m_renderPassFilter(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::MaterialParameterGathering, materialParameterGathererCounter++)
}

// TechniqueFilter / RenderPassFilter

// Parameters from Material/Effect/Technique

// Note: we could maybe improve that by having a smart update when we detect
// that a parameter value has changed. That might require way more book keeping
// which might make this solution a bit too complex

// The fact that this can now be performed in parallel should already provide a big
// improvement
void MaterialParameterGathererJob::run()
{
    for (const HMaterial &materialHandle : std::as_const(m_handles)) {
        Material *material = m_manager->materialManager()->data(materialHandle);

        if (Q_UNLIKELY(!material->isEnabled()))
            continue;

        Effect *effect = m_manager->effectManager()->lookupResource(material->effect());
        Technique *technique = findTechniqueForEffect(m_manager, m_techniqueFilter, effect);

        if (Q_LIKELY(technique != nullptr)) {
            RenderPassList passes = findRenderPassesForTechnique(m_manager, m_renderPassFilter, technique);
            if (Q_LIKELY(passes.size() > 0)) {
                // Order set:
                // 1 Pass Filter
                // 2 Technique Filter
                // 3 Material
                // 4 Effect
                // 5 Technique
                // 6 RenderPass

                // Add Parameters define in techniqueFilter and passFilter
                // passFilter have priority over techniqueFilter

                ParameterInfoList parameters;
                // Doing the reserve allows a gain of 0.5ms on some of the demo examples
                parameters.reserve(likelyNumberOfParameters);

                if (m_renderPassFilter)
                    parametersFromParametersProvider(&parameters, m_manager->parameterManager(),
                                                     m_renderPassFilter);
                if (m_techniqueFilter)
                    parametersFromParametersProvider(&parameters, m_manager->parameterManager(),
                                                     m_techniqueFilter);
                // Get the parameters for our selected rendering setup (override what was defined in the technique/pass filter)
                parametersFromMaterialEffectTechnique(&parameters, m_manager->parameterManager(), material, effect, technique);

                for (RenderPass *renderPass : passes) {
                    ParameterInfoList globalParameters = parameters;
                    parametersFromParametersProvider(&globalParameters, m_manager->parameterManager(), renderPass);
                    auto it = m_parameters.find(material->peerId());
                    if (it != m_parameters.end())
                        it->push_back({renderPass, globalParameters});
                    else
                        m_parameters.insert(material->peerId(), {{renderPass, globalParameters}});
                }
            }
        }
    }
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
