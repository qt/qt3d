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

bool FilterKey::operator ==(const FilterKey &other)
{
    if (&other == this)
        return true;
    // TODO create a QVaraint::fastCompare function that returns false
    // if types are not equal. For now, applying
    // https://codereview.qt-project.org/#/c/204484/
    // and adding the following early comparison of the types should give
    // an equivalent performance gain:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return (other.value().metaType() == value().metaType() &&
#else
    return (other.value().type() == value().type() &&
#endif
            other.name() == name() &&
            other.value() == value());
}

bool FilterKey::operator !=(const FilterKey &other)
{
    return !operator ==(other);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
