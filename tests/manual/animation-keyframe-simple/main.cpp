// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <Qt3DAnimation/QAnimationAspect>
#include <Qt3DQuickExtras/Qt3DQuickWindow>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QQmlEngine>
#include <QQmlContext>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.registerAspect(new Qt3DAnimation::QAnimationAspect());
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();
    return app.exec();
}
