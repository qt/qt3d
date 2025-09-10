// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DANIMATION_ANIMATION_ANIMATIONLOGGING_P_H
#define QT3DANIMATION_ANIMATION_ANIMATIONLOGGING_P_H

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

#include <QLoggingCategory>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

Q_DECLARE_LOGGING_CATEGORY(Backend)
Q_DECLARE_LOGGING_CATEGORY(Frontend)
Q_DECLARE_LOGGING_CATEGORY(Jobs)
Q_DECLARE_LOGGING_CATEGORY(HandlerLogic)

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_ANIMATIONLOGGING_P_H
