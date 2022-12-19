// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "filterkey_p.h"
#include <Qt3DRender/private/qfilterkey_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

FilterKey::FilterKey()
    : BackendNode()
{
}

FilterKey::~FilterKey()
{
    cleanup();
}

void FilterKey::cleanup()
{
    QBackendNode::setEnabled(false);
    m_name.clear();
    m_value.clear();
}

void FilterKey::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QFilterKey *node = qobject_cast<const QFilterKey *>(frontEnd);
    if (!node)
        return;

    BackendNode::syncFromFrontEnd(frontEnd, firstTime);

    if (node->name() != m_name) {
        m_name = node->name();
        markDirty(AbstractRenderer::AllDirty);
    }

    if (node->value() != m_value) {
        m_value = node->value();
        markDirty(AbstractRenderer::AllDirty);
    }
}

bool FilterKey::equals(const FilterKey &other) const
{
    if (&other == this)
        return true;
    // TODO create a QVaraint::fastCompare function that returns false
    // if types are not equal. For now, applying
    // https://codereview.qt-project.org/#/c/204484/
    // and adding the following early comparison of the types should give
    // an equivalent performance gain:
    return (other.value().metaType() == value().metaType() &&
            other.name() == name() &&
            other.value() == value());
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
