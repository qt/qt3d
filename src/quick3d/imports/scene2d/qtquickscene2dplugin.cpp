/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

    // Auto-increment the import to stay in sync with ALL future Qt minor versions
    qmlRegisterModule(uri, 2, QT_VERSION_MINOR);
}

QT_END_NAMESPACE
