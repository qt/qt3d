// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "parameter_p.h"
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/private/qparameter_p.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qshaderdata.h>
#include <Qt3DRender/private/buffer_p.h>

#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/stringtoint_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Parameter::Parameter()
    : BackendNode()
    , m_nameId(-1)
{
}

void Parameter::cleanup()
{
    QBackendNode::setEnabled(false);
    m_nameId = -1;
    m_name.clear();
    m_uniformValue = UniformValue();
    m_backendValue = {};
}

void Parameter::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QParameter *node = qobject_cast<const QParameter *>(frontEnd);
    if (!node)
        return;

    AbstractRenderer::BackendNodeDirtySet dirty = firstTime ? AbstractRenderer::ParameterDirty : static_cast<AbstractRenderer::BackendNodeDirtyFlag>(0);
    if (node->isEnabled() != isEnabled())
        dirty |= (AbstractRenderer::MaterialDirty | AbstractRenderer::ParameterDirty);

    if (node->name() != m_name) {
        m_name = node->name();
        m_nameId = StringToInt::lookupId(m_name);
        dirty |= (AbstractRenderer::MaterialDirty | AbstractRenderer::ParameterDirty);
    }

    QParameterPrivate* d = static_cast<QParameterPrivate *>(QParameterPrivate::get(const_cast<QParameter *>(node)));
    if (d->m_backendValue != m_backendValue) {
        m_backendValue = d->m_backendValue;
        m_uniformValue = UniformValue::fromVariant(m_backendValue);
        dirty |= (AbstractRenderer::ParameterDirty);
    }

    if (dirty)
        markDirty(dirty);

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
}

QString Parameter::name() const
{
    return m_name;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
