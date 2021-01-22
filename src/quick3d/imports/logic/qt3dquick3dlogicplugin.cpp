/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
**
**
**
****************************************************************************/

#include "qt3dquick3dlogicplugin.h"

#include <Qt3DLogic/qframeaction.h>
#include <QtQml>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DLogicPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Qt3DLogic::QFrameAction>(uri, 2, 0, "FrameAction");

    // Auto-increment the import to stay in sync with ALL future Qt minor versions
    qmlRegisterModule(uri, 2, QT_VERSION_MINOR);
}

QT_END_NAMESPACE
