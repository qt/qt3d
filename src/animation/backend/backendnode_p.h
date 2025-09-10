// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_BACKENDNODE_H
#define QT3DANIMATION_ANIMATION_BACKENDNODE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DAnimation/private/handler_p.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Q_AUTOTEST_EXPORT BackendNode : public Qt3DCore::QBackendNode
{
public:
    BackendNode(Qt3DCore::QBackendNode::Mode mode = ReadOnly);
    ~BackendNode();

    void setHandler(Handler *handler);

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

protected:
    void setDirty(Handler::DirtyFlag flag);
    Handler *m_handler;
};

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_BACKENDNODE_H
