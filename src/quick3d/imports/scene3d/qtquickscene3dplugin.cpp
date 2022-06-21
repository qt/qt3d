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
******************************************************************************/

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
