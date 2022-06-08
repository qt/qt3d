// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DAnimation/QAnimationAspect>
#include <QGuiApplication>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.registerAspect(new Qt3DAnimation::QAnimationAspect());

    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
