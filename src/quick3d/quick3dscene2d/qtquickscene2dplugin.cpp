// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtquickscene2dplugin.h"
#include "qt3dquickscene2d_global_p.h"

#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

Q_GHS_KEEP_REFERENCE(qml_register_types_QtQuick_Scene2D);
Q_GHS_KEEP_REFERENCE(Qt3DRender::initScene2dPlugin);

void QtQuickScene2DPlugin::registerTypes(const char *)
{
    volatile auto registration = &qml_register_types_QtQuick_Scene2D;
    volatile auto initialization = &Qt3DRender::initScene2dPlugin;
    Q_UNUSED(registration);
    Q_UNUSED(initialization);
}

QT_END_NAMESPACE

#include "moc_qtquickscene2dplugin.cpp"
