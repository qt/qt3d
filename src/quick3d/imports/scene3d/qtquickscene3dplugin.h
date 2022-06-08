// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QTQUICKSCENE3DPLUGIN_H
#define QTQUICKSCENE3DPLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

QT_BEGIN_NAMESPACE

class QtQuickScene3DPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    QtQuickScene3DPlugin(QObject *parent = 0) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri) override;
};

QT_END_NAMESPACE

#endif // QTQUICKSCENE3DPLUGIN_H
