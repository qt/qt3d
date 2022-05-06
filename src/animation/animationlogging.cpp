/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

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
