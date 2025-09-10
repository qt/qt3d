// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquickextrasnodefactory_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

Q_GLOBAL_STATIC(QuickExtrasNodeFactory, quick_extras_node_factory)

QuickExtrasNodeFactory *QuickExtrasNodeFactory::instance()
{
    return quick_extras_node_factory();
}

void QuickExtrasNodeFactory::registerType(const char *className, const char *quickName, int major, int minor)
{
    m_types.insert(className, Type(quickName, major, minor));
}

Qt3DCore::QNode *QuickExtrasNodeFactory::createNode(const char *type)
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

} // namespace Qt3DExtras

QT_END_NAMESPACE
