// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "parameterpack_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

ParameterPack::ParameterPack()
{
}

ParameterPack::~ParameterPack()
{
}

void ParameterPack::clear()
{
    m_peers.clear();
}

void ParameterPack::appendParameter(Qt3DCore::QNodeId parameterId)
{
    if (!m_peers.contains(parameterId)) {
        m_peers.append(parameterId);
    }
}

void ParameterPack::removeParameter(Qt3DCore::QNodeId parameterId)
{
    m_peers.removeOne(parameterId);
}

QList<Qt3DCore::QNodeId> ParameterPack::parameters() const
{
    return m_peers;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
