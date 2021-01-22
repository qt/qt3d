/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "qt3dquickscene2dnodefactory_p.h"

#include <private/qrenderaspect_p.h>

#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_GLOBAL_STATIC(QuickScene2DNodeFactory, quick_scene2d_node_factory)

static void initScene2dPlugin()
{
    Qt3DRender::QRenderAspectPrivate::configurePlugin(QLatin1String("scene2d"));
}

Q_COREAPP_STARTUP_FUNCTION(initScene2dPlugin)

QuickScene2DNodeFactory *QuickScene2DNodeFactory::instance()
{
    return quick_scene2d_node_factory();
}

void QuickScene2DNodeFactory::registerType(const char *className, const char *quickName,
                                           int major, int minor)
{
    m_types.insert(className, Type(quickName, major, minor));
}

Qt3DCore::QNode *QuickScene2DNodeFactory::createNode(const char *type)
{
    if (!m_types.contains(type))
        return nullptr;

    Type &typeInfo(m_types[type]);

    if (!typeInfo.resolved) {
        typeInfo.resolved = true;
        typeInfo.t = QQmlMetaType::qmlType(QString::fromLatin1(typeInfo.quickName),
                                           typeInfo.version.first, typeInfo.version.second);
    }

    return typeInfo.t.isValid() ? qobject_cast<Qt3DCore::QNode *>(typeInfo.t.create()) : nullptr;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
