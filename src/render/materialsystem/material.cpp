// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "material_p.h"
#include "technique_p.h"
#include "effect_p.h"
#include "qparameter.h"
#include "qtechnique.h"
#include "qmaterial.h"
#include "qeffect.h"
#include <Qt3DRender/private/qmaterial_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Material::Material()
    : BackendNode()
{
}

Material::~Material()
{
    cleanup();
}

void Material::cleanup()
{
    QBackendNode::setEnabled(false);
    m_parameterPack.clear();
}

void Material::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QMaterial *node = qobject_cast<const QMaterial *>(frontEnd);
    if (!node)
        return;

    AbstractRenderer::BackendNodeDirtySet dirty = firstTime ? AbstractRenderer::MaterialDirty : static_cast<AbstractRenderer::BackendNodeDirtyFlag>(0);

    auto parameters = qIdsForNodes(node->parameters());
    std::sort(std::begin(parameters), std::end(parameters));
    if (m_parameterPack.parameters() != parameters) {
        m_parameterPack.setParameters(parameters);
        dirty |= AbstractRenderer::AllDirty;
    }

    const auto effectId = node->effect() ? node->effect()->id() : QNodeId{};
    if (effectId != m_effectUuid) {
        m_effectUuid = effectId;
        dirty |= AbstractRenderer::AllDirty;
    }

    if (dirty)
        markDirty(dirty);
}

QList<Qt3DCore::QNodeId> Material::parameters() const
{
    return m_parameterPack.parameters();
}

Qt3DCore::QNodeId Material::effect() const
{
    return m_effectUuid;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
