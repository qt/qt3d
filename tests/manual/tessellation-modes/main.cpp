// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "tessellatedquadmesh.h"
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QGuiApplication>
#include <qqml.h>
#include <QQmlContext>
#include <QQmlEngine>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Qt3DExtras::Quick::Qt3DQuickWindow view;
    // Register our custom types
    qmlRegisterType<TessellatedQuadMesh>("Qt3D.Examples", 1, 0, "TessellatedQuadMesh");

    // Expose the window as a context property so we can set the aspect ratio
    view.engine()->qmlEngine()->rootContext()->setContextProperty("_window", &view);

    // There should be some synchronising mechanism to make sure
    // the source is set after all aspects have been completely initialized
    // Otherwise we might encounter cases where an Aspect's QML elements have
    // not yet been registered
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
