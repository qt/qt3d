// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQuickView>

#include <Qt3DRender/qt3drender-config.h>


int main(int argc, char **argv)
{
    {
        // Set OpenGL requirements
        QSurfaceFormat format = QSurfaceFormat::defaultFormat();
#if !QT_CONFIG(opengles2)
        format.setVersion(4, 1);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(4);
#else
        format.setVersion(3, 0);
        format.setProfile(QSurfaceFormat::NoProfile);
        format.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
        QSurfaceFormat::setDefaultFormat(format);
    }

#if !QT_CONFIG(qt3d_rhi_renderer)
    qputenv("QSG_RHI_BACKEND", "opengl");
#endif

    QGuiApplication app(argc, argv);

    // Force OpenGL backend
    QQuickView view;

    view.resize(1024, 768);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setPersistentSceneGraph(true);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
