// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QTQUICKSCENE2DPLUGIN_H
#define QTQUICKSCENE2DPLUGIN_H

#include <QtQml/qqmlextensionplugin.h>

QT_BEGIN_NAMESPACE

class QtQuickScene2DPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    QtQuickScene2DPlugin(QObject *parent = 0) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri) override;
};

QT_END_NAMESPACE

#endif // QTQUICKSCENE2DPLUGIN_H
