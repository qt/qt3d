// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICK3DEXTRASPLUGIN_H
#define QT3DQUICK3DEXTRASPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

static void initResources()
{
#ifdef QT_STATIC
    Q_INIT_RESOURCE(extras); // from the Qt3DQuickExtras module
#endif
}

QT_BEGIN_NAMESPACE

class Qt3DQuick3DExtrasPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    Qt3DQuick3DExtrasPlugin(QObject *parent = nullptr) : QQmlExtensionPlugin(parent) { initResources(); }
    void registerTypes(const char *uri) override;
};

QT_END_NAMESPACE

#endif // QT3DQUICK3DEXTRASPLUGIN_H
