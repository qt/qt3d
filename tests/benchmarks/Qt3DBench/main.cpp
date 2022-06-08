// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#if 0
// This one uses Scene3D
#include <QGuiApplication>
#include <QQuickView>
#include <QOpenGLContext>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        //format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setDepthBufferSize(24);
    format.setSamples(4);

    QQuickView view;
    view.resize(1280, 768);
    view.setFormat(format);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/Qt3DBenchMain.qml"));
    view.show();

    return app.exec();
}

#else

#include <window.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DQuick/QQmlAspectEngine>

#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Window view;
    Qt3DCore::Quick::QQmlAspectEngine engine;

    view.resize(1280, 768);
    engine.aspectEngine()->registerAspect(new Qt3DRender::QRenderAspect());
    engine.aspectEngine()->registerAspect(new Qt3DInput::QInputAspect());
    engine.aspectEngine()->initialize();
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.aspectEngine()->setData(data);
    engine.qmlEngine()->rootContext()->setContextProperty("_window", &view);
    engine.setSource(QUrl("qrc:/SphereView.qml"));

    view.show();

    return app.exec();
}

#endif
