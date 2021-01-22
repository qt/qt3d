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
****************************************************************************/

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
