// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <Qt3DRender/qt3drender-config.h>

int main(int argc, char **argv)
{
    const bool useUnderLayCompositingMode = false;

    if (useUnderLayCompositingMode) {
        // underLay compositing does not work with RHI renderer
        qputenv("QT3D_RENDERER", "opengl");
        qputenv("QSG_RHI_BACKEND", "opengl");
    }

    QSurfaceFormat format;
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);
    QQuickView view;

    view.resize(520, 500);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("_useUnderLayCompositingMode", useUnderLayCompositingMode);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
