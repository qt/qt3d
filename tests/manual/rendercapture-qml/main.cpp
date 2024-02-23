// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QQuickItem>
#include <QQuickView>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include "rendercaptureprovider.h"


int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQuickView view;
    RenderCaptureProvider *provider = new RenderCaptureProvider;
    qmlRegisterType<RenderCaptureProvider>("Extras", 1, 0, "RenderCaptureProvider");

    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    view.setFormat(format);

    view.engine()->rootContext()->setContextProperty("_renderCaptureProvider", provider);
    view.engine()->addImageProvider("rendercapture", provider);

    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
