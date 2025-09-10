// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_BACKENDNODE_H
#define QT3DINPUT_INPUT_BACKENDNODE_H

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

#include <Qt3DInput/private/qt3dinput_global_p.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class Q_3DINPUTSHARED_PRIVATE_EXPORT BackendNode : public Qt3DCore::QBackendNode
{
public:
    BackendNode(Qt3DCore::QBackendNode::Mode mode = ReadOnly);
    ~BackendNode();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

protected:
    explicit BackendNode(Qt3DCore::QBackendNodePrivate &dd);
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_BACKENDNODE_H
