// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DCore/QEntity>

#include <QGuiApplication>
#include <qqml.h>

class SceneHelper : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE QObject *findEntity(Qt3DRender::QSceneLoader *loader, const QString &name);
    Q_INVOKABLE QObject *findComponent(Qt3DCore::QEntity *entity, const QString &componentMetatype);
    Q_INVOKABLE void addListEntry(const QVariant &list, QObject *entry);
};

QObject *SceneHelper::findEntity(Qt3DRender::QSceneLoader *loader, const QString &name)
{
    // The QSceneLoader instance is a component of an entity. The loaded scene
    // tree is added under this entity.
    QList<Qt3DCore::QEntity *> entities = loader->entities();

    if (entities.isEmpty())
        return 0;

    // Technically there could be multiple entities referencing the scene loader
    // but sharing is discouraged, and in our case there will be one anyhow.
    Qt3DCore::QEntity *root = entities[0];

    // The scene structure and names always depend on the asset.
    return root->findChild<Qt3DCore::QEntity *>(name);
}

QObject *SceneHelper::findComponent(Qt3DCore::QEntity *entity, const QString &componentMetatype)
{
    Q_ASSERT(entity);
    const Qt3DCore::QComponentVector components = entity->components();
    for (Qt3DCore::QComponent *component : components) {
        qDebug() << component->metaObject()->className();
        if (component->metaObject()->className() == componentMetatype) {
            return component;
        }
    }
    return nullptr;
}

void SceneHelper::addListEntry(const QVariant &list, QObject *entry)
{
    QQmlListReference ref = list.value<QQmlListReference>();
    ref.append(entry);
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Qt3DExtras::Quick::Qt3DQuickWindow view;

    qmlRegisterType<SceneHelper>("Qt3D.Examples", 2, 0, "SceneHelper");
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}

#include "main.moc"
