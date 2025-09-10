// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DLOGIC_LOGIC_MANAGERS_P_H
#define QT3DLOGIC_LOGIC_MANAGERS_P_H

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

#include <QtCore/QtGlobal>

#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DLogic/private/handle_types_p.h>
#include <Qt3DLogic/private/handler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {
namespace Logic {

class HandlerManager : public Qt3DCore::QResourceManager<
        Handler,
        Qt3DCore::QNodeId>
{
public:
    HandlerManager() {}
};

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_MANAGERS_P_H

