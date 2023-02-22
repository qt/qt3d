// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>

#ifdef Q_OS_IOS
#include "iosdeviceorientation.h"
#else
#include "dummydeviceorientation.h"
#endif

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Qt3DExtras::Quick::Qt3DQuickWindow view;

#ifdef Q_OS_IOS
    qmlRegisterType<iOSDeviceOrientation>("Qardboard", 1, 0, "DeviceOrientation");
#else
    qmlRegisterType<DummyDeviceOrientation>("Qardboard", 1, 0, "DeviceOrientation");
#endif

    view.engine()->qmlEngine()->rootContext()->setContextProperty(QStringLiteral("_window"), &view);
    view.setSource(QUrl("qrc:/main.qml"));
#ifdef Q_OS_IOS
    view.showFullScreen();
#else
    view.resize(800, 400);
    view.show();
#endif

    return app.exec();
}
