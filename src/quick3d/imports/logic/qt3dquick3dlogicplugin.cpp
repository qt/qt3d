// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquick3dlogicplugin.h"

#include <Qt3DLogic/qframeaction.h>
#include <QtQml>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DLogicPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Qt3DLogic::QFrameAction>(uri, 2, 0, "FrameAction");

    // The minor version used to be the current Qt 5 minor. For compatibility it is the last
    // Qt 5 release.
    qmlRegisterModule(uri, 2, 15);
}

QT_END_NAMESPACE

#include "moc_qt3dquick3dlogicplugin.cpp"
