/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <window.h>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DCore/QEntity>
#include <Qt3DInput/QInputAspect>
#include <Qt3DQuick/QQmlAspectEngine>

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
    QVector<Qt3DCore::QEntity *> entities = loader->entities();

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
    Qt3DCore::QComponentList components = entity->components();
    Q_FOREACH (Qt3DCore::QComponent *component, components) {
        qDebug() << component->metaObject()->className();
        if (component->metaObject()->className() == componentMetatype) {
            return component;
        }
    }
    return Q_NULLPTR;
}

void SceneHelper::addListEntry(const QVariant &list, QObject *entry)
{
    QQmlListReference ref = list.value<QQmlListReference>();
    ref.append(entry);
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Window view;
    Qt3DCore::Quick::QQmlAspectEngine engine;

    engine.aspectEngine()->registerAspect(new Qt3DRender::QRenderAspect());
    engine.aspectEngine()->registerAspect(new Qt3DInput::QInputAspect());
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.aspectEngine()->setData(data);
    qmlRegisterType<SceneHelper>("Qt3D.Examples", 2, 0, "SceneHelper");
    engine.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}

#include "main.moc"
