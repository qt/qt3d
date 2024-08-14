// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquick3drenderplugin.h"

#include <QtGui/qwindow.h>

#include <Qt3DQuickRender/private/quick3dshaderdata_p.h>
#include <Qt3DQuickRender/private/quick3dshaderdataarray_p.h>

QT_BEGIN_NAMESPACE

Q_GHS_KEEP_REFERENCE(qml_register_types_Qt3D_Render);
Q_GHS_KEEP_REFERENCE(Qt3DRender::Quick::Quick3DRender_initialize);

void Qt3DQuick3DRenderPlugin::registerTypes(const char *)
{
    volatile auto registerFunction = &qml_register_types_Qt3D_Render;
    volatile auto initializeFunction = &Qt3DRender::Quick::Quick3DRender_initialize;
    Q_UNUSED(registerFunction);
    Q_UNUSED(initializeFunction);
}

QT_END_NAMESPACE

#include "moc_qt3dquick3drenderplugin.cpp"


