/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

#include "backendnode_p.h"
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qbackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

BackendNode::BackendNode(Mode mode)
    : Qt3DCore::QBackendNode(mode)
{
}

BackendNode::BackendNode(Qt3DCore::QBackendNodePrivate &dd)
    : Qt3DCore::QBackendNode(dd)
{

}

BackendNode::~BackendNode()
{
}

void BackendNode::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    Q_UNUSED(firstTime)

    d_ptr->setEnabled(frontEnd->isEnabled());
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
