// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQuickView>
#include <Qt3DRender/qt3drender-config.h>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

#if !QT_CONFIG(qt3d_rhi_renderer)
    qputenv("QSG_RHI_BACKEND", "opengl");
#endif

    QQuickView view;

    view.resize(500, 500);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
