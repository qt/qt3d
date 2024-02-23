// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQuickView>
#include <QOpenGLContext>
#include <Qt3DRender/qt3drender-config.h>

void setSurfaceFormat()
{
    QSurfaceFormat format;
#if QT_CONFIG(opengles2)
    format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
#endif
    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

#if !QT_CONFIG(qt3d_rhi_renderer)
    qputenv("QSG_RHI_BACKEND", "opengl");
#endif
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    setSurfaceFormat();

    QQuickView view;

    view.resize(1920, 1080);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
