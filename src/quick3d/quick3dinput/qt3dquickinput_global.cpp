// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/qt3dquickinput_global_p.h>
#include <Qt3DQuickInput/private/qt3dquickinputnodefactory_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Quick {

void Quick3DInput_initialize()
{
    Qt3DCore::QAbstractNodeFactory::registerNodeFactory(QuickInputNodeFactory::instance());
}

void Quick3DInput_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickInputNodeFactory::instance()->registerType(className, quickName, major, minor);
}

} // namespace Quick
} // namespace Qt3DInput

QT_END_NAMESPACE
