// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquickanimationplugin_p.h"
#include "qt3dquickanimation_global_p.h"

QT_BEGIN_NAMESPACE

Q_GHS_KEEP_REFERENCE(qml_register_types_Qt3D_Animation);
Q_GHS_KEEP_REFERENCE(Qt3DAnimation::Quick::Quick3DAnimation_initialize);

void Qt3DQuick3DAnimationPlugin::registerTypes(const char *)
{
    volatile auto registerFunction = &qml_register_types_Qt3D_Animation;
    Q_UNUSED(registerFunction);
    volatile auto initializeFunction = &Qt3DAnimation::Quick::Quick3DAnimation_initialize;
    Q_UNUSED(initializeFunction);
}

QT_END_NAMESPACE
