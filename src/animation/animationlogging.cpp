// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "animationlogging_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

Q_LOGGING_CATEGORY(Backend, "Qt3D.Animation.Backend", QtWarningMsg)
Q_LOGGING_CATEGORY(Frontend, "Qt3D.Animation.Frontend", QtWarningMsg)
Q_LOGGING_CATEGORY(Jobs, "Qt3D.Animation.Jobs", QtWarningMsg)
Q_LOGGING_CATEGORY(HandlerLogic, "Qt3D.Animation.Handler", QtWarningMsg)

} // namespace Animation
} // namespace Qt3DAnimation

QT_END_NAMESPACE
