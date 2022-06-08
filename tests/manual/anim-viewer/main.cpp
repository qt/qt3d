// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuickExtras/qt3dquickwindow.h>

#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQuickView view;

    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
