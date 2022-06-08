// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QApplication>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickView>
#include <Qt3DRender/QAbstractTexture>
#include <QWindow>
#include <QTimer>

#include <QQuickView>
#include "videoplayer.h"


template<typename Obj>
QHash<int, QString> enumToNameMap(const char *enumName)
{
    const QMetaObject metaObj = Obj::staticMetaObject;
    const int indexOfEnum = metaObj.indexOfEnumerator(enumName);
    const QMetaEnum metaEnum = metaObj.enumerator(indexOfEnum);
    const int keysCount = metaEnum.keyCount();

    QHash<int, QString> v;
    v.reserve(keysCount);
    for (int i = 0; i < keysCount; ++i)
        v[metaEnum.value(i)] = metaEnum.key(i);
    return v;
}


class EnumNameMapper : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE QString statusName(int v) const { return m_statusMap.value(v); }
    Q_INVOKABLE QString formatName(int v) const { return m_formatMap.value(v); }
    Q_INVOKABLE QString targetName(int v) const { return m_targetMap.value(v); }

private:
    const QHash<int, QString> m_statusMap = enumToNameMap<Qt3DRender::QAbstractTexture>("Status");
    const QHash<int, QString> m_formatMap = enumToNameMap<Qt3DRender::QAbstractTexture>("TextureFormat");
    const QHash<int, QString> m_targetMap = enumToNameMap<Qt3DRender::QAbstractTexture>("Target");
};

int main(int argc, char* argv[])
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setMajorVersion(4);
    format.setMinorVersion(5);
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);


    // Make the OpenGLWidget's shared context be qt_gl_global_share_context
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);

    // Multimedia player
    TextureWidget textureWidget;
    VideoPlayer *videoPlayer = new VideoPlayer(&textureWidget);

    textureWidget.resize(800, 600);
    textureWidget.show();

    // Qt3D QtQuick Scene (uses qt_global_share_context by default)
    QQuickView view;
    QQmlContext *ctx = view.rootContext();
    EnumNameMapper mapper;
    ctx->setContextProperty(QStringLiteral("_nameMapper"), &mapper);
    ctx->setContextProperty(QStringLiteral("_textureWidget"), &textureWidget);

    view.resize(800, 600);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}

#include "main.moc"
