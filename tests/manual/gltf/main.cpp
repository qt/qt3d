// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <QGuiApplication>
#include <QOpenGLContext>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Quick::Qt3DQuickWindow view;

    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    const bool isES = QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGLES
        || view.requestedFormat().renderableType() == QSurfaceFormat::OpenGLES;
    if (!isES)
        qDebug("Using a non-OpenGL ES context. This may result in no material on the model "
               "as currently the standard glTF format only includes OpenGL ES 2.0 shaders. "
               "To overcome this, run the application on OpenGL ES or use the qgltf tool "
               "with -g to generate a slightly extended glTF asset from the original COLLADA source.");

    return app.exec();
}
