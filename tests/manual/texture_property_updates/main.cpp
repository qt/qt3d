// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QQuickView>
#include <QGuiApplication>
#include <QQmlContext>
#include <Qt3DRender/QAbstractTexture>

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
    Q_INVOKABLE QString handleTypeName(int v) const { return m_handleTypeMap.value(v); }

private:
    const QHash<int, QString> m_statusMap = enumToNameMap<Qt3DRender::QAbstractTexture>("Status");
    const QHash<int, QString> m_formatMap = enumToNameMap<Qt3DRender::QAbstractTexture>("TextureFormat");
    const QHash<int, QString> m_targetMap = enumToNameMap<Qt3DRender::QAbstractTexture>("Target");
    const QHash<int, QString> m_handleTypeMap = enumToNameMap<Qt3DRender::QAbstractTexture>("HandleType");
};

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQuickView view;

    QQmlContext *ctx =view.rootContext();
    EnumNameMapper mapper;
    ctx->setContextProperty(QStringLiteral("nameMapper"), &mapper);

    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}

#include "main.moc"
