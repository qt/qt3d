// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquickextras_global_p.h"

#include <Qt3DQuickExtras/private/qt3dquickextrasnodefactory_p.h>
#include <QtQml/private/qqmlglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {
namespace Quick {

void Quick3DExtras_initialize()
{
#ifdef QT_STATIC
    Q_INIT_RESOURCE(extras); // from the Qt3DQuickExtras module
#endif
    Qt3DCore::QAbstractNodeFactory::registerNodeFactory(QuickExtrasNodeFactory::instance());
    Qt3DExtras::Quick::Quick3DExtras_registerType("QSpriteSheet", "Qt3D.Extras/SpriteSheet", 2, 10);
}

void Quick3DExtras_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickExtrasNodeFactory::instance()->registerType(className, quickName, major, minor);
}

Q_CONSTRUCTOR_FUNCTION(Quick3DExtras_initialize)

} // namespace Quick
} // namespace Qt3DExtras

QT_END_NAMESPACE
