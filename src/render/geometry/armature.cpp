/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#include "armature_p.h"

#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qabstractskeleton.h>

#include <Qt3DCore/private/qarmature_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Armature::Armature()
    : BackendNode(Qt3DCore::QBackendNode::ReadOnly)
{
}

void Armature::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QArmature *node = qobject_cast<const QArmature *>(frontEnd);
    if (!node)
        return;

    m_skeletonId = node->skeleton() ? node->skeleton()->id() : QNodeId{};
}

void Armature::cleanup()
{
    m_skeletonId = Qt3DCore::QNodeId();
    setEnabled(false);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
