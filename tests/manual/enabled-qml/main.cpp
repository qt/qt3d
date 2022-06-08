// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <Qt3DQuickExtras/qt3dquickwindow.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Quick::Qt3DQuickWindow view;

    view.setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    view.show();

    return app.exec();
}
