// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause


#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <QGuiApplication>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Quick::Qt3DQuickWindow view;

    view.engine()->qmlEngine()->rootContext()->setContextProperty("_view", &view);
    view.setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    view.show();

    return app.exec();
}
