// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/effect_p.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/private/qeffect_p.h>

#include <QVariant>
#include <algorithm>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Effect::Effect()
    : BackendNode()
{
}

Effect::~Effect()
{
    cleanup();
}

void Effect::cleanup()
{
    QBackendNode::setEnabled(false);
    m_parameterPack.clear();
    m_techniques.clear();
}

void Effect::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QEffect *node = qobject_cast<const QEffect *>(frontEnd);
    if (!node)
        return;

    auto parameters = qIdsForNodes(node->parameters());
    std::sort(std::begin(parameters), std::end(parameters));
    if (m_parameterPack.parameters() != parameters)
        m_parameterPack.setParameters(parameters);

    auto techniques = qIdsForNodes(node->techniques());
    std::sort(std::begin(techniques), std::end(techniques));
    if (m_techniques != techniques)
        m_techniques = techniques;

    if (!firstTime)
        markDirty(AbstractRenderer::AllDirty);
}

void Effect::appendRenderTechnique(Qt3DCore::QNodeId technique)
{
    if (!m_techniques.contains(technique))
        m_techniques.append(technique);
}

QList<Qt3DCore::QNodeId> Effect::techniques() const
{
    return m_techniques;
}

QList<Qt3DCore::QNodeId> Effect::parameters() const
{
    return m_parameterPack.parameters();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
