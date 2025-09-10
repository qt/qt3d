// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickInput/private/qt3dquickinputnodefactory_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

Q_GLOBAL_STATIC(QuickInputNodeFactory, quick_Input_node_factory)

QuickInputNodeFactory *QuickInputNodeFactory::instance()
{
    return quick_Input_node_factory();
}

void QuickInputNodeFactory::registerType(const char *className, const char *quickName, int major, int minor)
{
    m_types.insert(className, Type(quickName, major, minor));
}

Qt3DCore::QNode *QuickInputNodeFactory::createNode(const char *type)
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

} // namespace Qt3DInput

QT_END_NAMESPACE
