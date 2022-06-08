// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>
#include <Qt3DQuickRender/private/qt3dquickrendernodefactory_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Quick {

void Quick3DRender_initialize()
{
    Qt3DCore::QAbstractNodeFactory::registerNodeFactory(QuickRenderNodeFactory::instance());
}

void Quick3DRender_registerType(const char *className, const char *quickName, int major, int minor)
{
    QuickRenderNodeFactory::instance()->registerType(className, quickName, major, minor);
}

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE
