// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtquickscene2dplugin.h"

#include <Qt3DQuickScene2D/qscene2d.h>
#include <private/qrenderaspect_p.h>
#include <Qt3DQuickScene2D/private/qt3dquick3dscene2d_p.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

static void initScene2dPlugin()
{
    Qt3DRender::QRenderAspectPrivate::configurePlugin("scene2d");
}

Q_COREAPP_STARTUP_FUNCTION(initScene2dPlugin)

void QtQuickScene2DPlugin::registerTypes(const char *uri)
{
    qmlRegisterExtendedType<Qt3DRender::Quick::QScene2D, Qt3DRender::Render::Quick::QQuick3DScene2D>(uri, 2, 9, "Scene2D");

    // The minor version used to be the current Qt 5 minor. For compatibility it is the last
    // Qt 5 release.
    qmlRegisterModule(uri, 2, 15);
}

QT_END_NAMESPACE

#include "moc_qtquickscene2dplugin.cpp"
