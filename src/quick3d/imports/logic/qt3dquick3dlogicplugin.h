// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICK3DLOGICPLUGIN_H
#define QT3DQUICK3DLOGICPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

QT_BEGIN_NAMESPACE

class Qt3DQuick3DLogicPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    Qt3DQuick3DLogicPlugin(QObject *parent = 0) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri) override;
};

QT_END_NAMESPACE

#endif // QT3DQUICK3DLOGICPLUGIN_H
