// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquickscene2d_global_p.h"
#include "qt3dquickscene2dnodefactory_p.h"

#include <private/qrenderaspect_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
void initScene2dPlugin()
{
    Qt3DRender::QRenderAspectPrivate::configurePlugin(QLatin1String("scene2d"));
}

Q_COREAPP_STARTUP_FUNCTION(initScene2dPlugin)

namespace Quick {


void Quick3DScene2D_initialize()
{
    Qt3DCore::QAbstractNodeFactory::registerNodeFactory(QuickScene2DNodeFactory::instance());
}

void Quick3DScene2D_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickScene2DNodeFactory::instance()->registerType(className, quickName, major, minor);
}

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE
