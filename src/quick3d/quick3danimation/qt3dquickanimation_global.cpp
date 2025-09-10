// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquickanimation_global_p.h"
#include "qt3dquickanimationnodefactory_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Quick {

void Quick3DAnimation_initialize()
{
    Qt3DCore::QAbstractNodeFactory::registerNodeFactory(QuickAnimationNodeFactory::instance());
}

void Quick3DAnimation_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickAnimationNodeFactory::instance()->registerType(className, quickName, major, minor);
}

Q_CONSTRUCTOR_FUNCTION(Quick3DAnimation_initialize)

} // namespace Quick
} // namespace Qt3DAnimation

QT_END_NAMESPACE
