// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquick3dextrasplugin_p.h"
#include "qt3dquick3dextrasforeign_p.h"

#include <Qt3DExtras/qspritesheetitem.h>
#include <Qt3DQuickExtras/private/qt3dquickextras_global_p.h>
#include <Qt3DQuickExtras/private/quick3dspritesheet_p.h>
#include <Qt3DQuickExtras/private/qt3dquickextras_global_p.h>

#include <QtQml/qqml.h>
#include <QtQml/qqmlextensionplugin.h>

#include <QtCore/qtsymbolmacros.h>

QT_BEGIN_NAMESPACE

QT_DECLARE_EXTERN_SYMBOL(qml_register_types_Qt3D_Extras, void);
QT_DECLARE_EXTERN_SYMBOL(Qt3DExtras::Quick::Quick3DExtras_initialize, void);

void Qt3DQuick3DExtrasPlugin::registerTypes(const char *)
{
    volatile auto registerFunction = &qml_register_types_Qt3D_Extras;
    Q_UNUSED(registerFunction);

    volatile auto initializeFunction = &Qt3DExtras::Quick::Quick3DExtras_initialize;
    Q_UNUSED(initializeFunction);
}

QT_END_NAMESPACE
