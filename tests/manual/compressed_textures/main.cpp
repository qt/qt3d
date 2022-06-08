// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QQuickView>
#include <QGuiApplication>
#include <QQmlContext>
#include <QDir>

QStringList createTextureFileList()
{
    QDir dir(QString::fromUtf8(DATA_DIR));
    return dir.entryList();
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQuickView view;

    QQmlContext *ctx =view.rootContext();
    ctx->setContextProperty(QStringLiteral("_pathPrefix"), QString::fromUtf8(DATA_DIR));
    ctx->setContextProperty(QStringLiteral("_texturesList"), createTextureFileList());

    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
