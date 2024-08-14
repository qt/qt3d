// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquickscene2dnodefactory_p.h"

#include <private/qrenderaspect_p.h>

#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_GLOBAL_STATIC(QuickScene2DNodeFactory, quick_scene2d_node_factory)

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
        typeInfo.t = QQmlMetaType::qmlType(QString::fromLatin1(typeInfo.quickName), QTypeRevision::fromVersion(typeInfo.version.first, typeInfo.version.second));
    }

    return typeInfo.t.isValid() ? qobject_cast<Qt3DCore::QNode *>(typeInfo.t.create()) : nullptr;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
