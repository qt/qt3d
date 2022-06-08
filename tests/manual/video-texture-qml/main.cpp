// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuick/QQmlAspectEngine>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <Qt3DQuickExtras/Qt3DQuickWindow>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Quick::Qt3DQuickWindow view;

    view.engine()->qmlEngine()->rootContext()->setContextProperty("_window", &view);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
