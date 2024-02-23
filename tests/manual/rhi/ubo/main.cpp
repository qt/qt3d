// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <Qt3DQuickExtras/qt3dquickwindow.h>

int main(int ac, char **av)
{
    qputenv("QT3D_RENDERER", "rhi");
    QGuiApplication app(ac, av);
    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
