// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQuickView>
#include <Qt3DRender/qt3drender-config.h>

int main(int argc, char **argv)
{
    QSurfaceFormat format;
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
#if !QT_CONFIG(qt3d_rhi_renderer)
    qputenv("QSG_RHI_BACKEND", "opengl");
#endif

    QGuiApplication app(argc, argv);

    QQuickView view;

    view.resize(520, 500);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
