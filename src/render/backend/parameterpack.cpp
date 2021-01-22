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

QVector<Qt3DCore::QNodeId> ParameterPack::parameters() const
{
    return m_peers;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
