/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "material_p.h"
#include "technique_p.h"
#include "effect_p.h"
#include "qparameter.h"
#include "qtechnique.h"
#include "qmaterial.h"
#include "qeffect.h"
#include <Qt3DRender/private/qmaterial_p.h>

using namespace Qt3DCore;

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

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
