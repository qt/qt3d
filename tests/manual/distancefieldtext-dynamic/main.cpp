// Copyright (C) 2023 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQuickView>
#include <QOpenGLContext>

int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");
    qputenv("QT3D_RENDERER", "opengl");

    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
    }

    QQuickView view;
    view.setFormat(format);
    view.resize(1200, 600);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
