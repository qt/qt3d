// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DLOGIC_LOGIC_HANDLE_TYPES_P_H
#define QT3DLOGIC_LOGIC_HANDLE_TYPES_P_H

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

#include <Qt3DCore/private/qhandle_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {
namespace Logic {

class Handler;
typedef Qt3DCore::QHandle<Handler> HHandler;

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_HANDLE_TYPES_P_H

