// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "layer_p.h"
#include <Qt3DRender/qlayer.h>
#include <Qt3DRender/private/qlayer_p.h>
#include <Qt3DRender/private/stringtoint_p.h>
#include <QVariant>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Layer::Layer()
    : BackendNode()
    , m_recursive(false)
{
}

Layer::~Layer()
{
    cleanup();
}

void Layer::cleanup()
{
    QBackendNode::setEnabled(false);
}

void Layer::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    const QLayer *node = qobject_cast<const QLayer *>(frontEnd);
    if (!node)
        return;

    const bool oldEnabled = isEnabled();
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (isEnabled() != oldEnabled || firstTime)
        markDirty(AbstractRenderer::LayersDirty);

    if (node->recursive() != m_recursive) {
        m_recursive = node->recursive();
        markDirty(AbstractRenderer::LayersDirty);
    }
}

bool Layer::recursive() const
{
    return m_recursive;
}

void Layer::setRecursive(bool recursive)
{
    m_recursive = recursive;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
