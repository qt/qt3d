// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICK3DANIMATIONPLUGIN_H
#define QT3DQUICK3DANIMATIONPLUGIN_H

#include <QtQml/qqmlextensionplugin.h>

QT_BEGIN_NAMESPACE

class Qt3DQuick3DAnimationPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")
public:
    Qt3DQuick3DAnimationPlugin(QObject *parent = 0) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri) override;
};

QT_END_NAMESPACE

#endif // QT3DQUICK3DANIMATIONPLUGIN_H
