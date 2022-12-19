// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickRender/private/qt3dquickrendernodefactory_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_GLOBAL_STATIC(QuickRenderNodeFactory, quick_render_node_factory)

QuickRenderNodeFactory *QuickRenderNodeFactory::instance()
{
    return quick_render_node_factory();
}

void QuickRenderNodeFactory::registerType(const char *className, const char *quickName, int major, int minor)
{
    m_types.insert(className, Type(quickName, major, minor));
}

Qt3DCore::QNode *QuickRenderNodeFactory::createNode(const char *type)
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
