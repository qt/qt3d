// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtquickscene3dplugin.h"

#include <QtQml>

#include <scene3ditem_p.h>

QT_BEGIN_NAMESPACE

void QtQuickScene3DPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Qt3DRender::Scene3DItem>(uri, 2, 0, "Scene3D");
    qmlRegisterType<Qt3DRender::Scene3DItem, 14>(uri, 2, 14, "Scene3D");

    // The minor version used to be the current Qt 5 minor. For compatibility it is the last
    // Qt 5 release.
    qmlRegisterModule(uri, 2, 15);
}

QT_END_NAMESPACE

#include "moc_qtquickscene3dplugin.cpp"
