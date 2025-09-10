// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "corelogging_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

Q_LOGGING_CATEGORY(Nodes, "Qt3D.Core.Nodes", QtWarningMsg);
Q_LOGGING_CATEGORY(Aspects, "Qt3D.Core.Aspects", QtWarningMsg);
Q_LOGGING_CATEGORY(Resources, "Qt3D.Core.Resources", QtWarningMsg);
Q_LOGGING_CATEGORY(ChangeArbiter, "Qt3D.Core.ChangeArbiter", QtWarningMsg);

} // Qt3D

QT_END_NAMESPACE
