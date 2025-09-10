// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DCORE_CORELOGGING_P_H
#define QT3DCORE_CORELOGGING_P_H

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

#include <QtCore/QLoggingCategory>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

Q_DECLARE_LOGGING_CATEGORY(Resources)
Q_DECLARE_LOGGING_CATEGORY(Nodes)
Q_DECLARE_LOGGING_CATEGORY(Aspects)
Q_DECLARE_LOGGING_CATEGORY(ChangeArbiter)


} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_CORELOGGING_P_H
