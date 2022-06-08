// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QFileDialog>
#include <QApplication>

#include <Qt3DRender/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DInput/QInputAspect>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>

class SceneWalker : public QObject
{
public:
    SceneWalker(Qt3DRender::QSceneLoader *loader) : m_loader(loader) { }

    void onStatusChanged();

private:
    void walkEntity(Qt3DCore::QEntity *e, int depth = 0);

    Qt3DRender::QSceneLoader *m_loader;
};

void SceneWalker::onStatusChanged()
{
    qDebug() << "Status changed:" << m_loader->status();
    if (m_loader->status() != Qt3DRender::QSceneLoader::Ready)
        return;

    // The QSceneLoader instance is a component of an entity. The loaded scene
    // tree is added under this entity.
    QList<Qt3DCore::QEntity *> entities = m_loader->entities();

    // Technically there could be multiple entities referencing the scene loader
    // but sharing is discouraged, and in our case there will be one anyhow.
    if (entities.isEmpty())
        return;
    Qt3DCore::QEntity *root = entities[0];
    // Print the tree.
    walkEntity(root);

    // To access a given node (like a named mesh in the scene), use QObject::findChild().
    // The scene structure and names always depend on the asset.
    Qt3DCore::QEntity *e = root->findChild<Qt3DCore::QEntity *>(QStringLiteral("PlanePropeller_mesh")); // toyplane.obj
    if (e)
        qDebug() << "Found propeller node" << e << "with components" << e->components();
}

void SceneWalker::walkEntity(Qt3DCore::QEntity *e, int depth)
{
    Qt3DCore::QNodeVector nodes = e->childNodes();
    for (int i = 0; i < nodes.count(); ++i) {
        Qt3DCore::QNode *node = nodes[i];
        Qt3DCore::QEntity *entity = qobject_cast<Qt3DCore::QEntity *>(node);
        if (entity) {
            QString indent;
            indent.fill(' ', depth * 2);
            qDebug().noquote() << indent << "Entity:" << entity << "Components:" << entity->components();
            walkEntity(entity, depth + 1);
        }
    }
}

int main(int ac, char **av)
{
    QApplication app(ac, av);
    Qt3DExtras::Qt3DWindow view;
    view.defaultFrameGraph()->setClearColor(Qt::black);

    // Root entity
    Qt3DCore::QEntity *sceneRoot = new Qt3DCore::QEntity();

    // Scene Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setViewCenter(QVector3D(0.0f, 3.5f, 0.0f));
    camera->setPosition(QVector3D(0.0f, 3.5f, 25.0f));
    camera->setNearPlane(0.001f);
    camera->setFarPlane(10000.0f);

    // For camera controls
    Qt3DExtras::QFirstPersonCameraController *camController = new Qt3DExtras::QFirstPersonCameraController(sceneRoot);
    camController->setCamera(camera);

    // Scene loader
    Qt3DCore::QEntity *sceneLoaderEntity = new Qt3DCore::QEntity(sceneRoot);
    Qt3DRender::QSceneLoader *sceneLoader = new Qt3DRender::QSceneLoader(sceneLoaderEntity);
    SceneWalker sceneWalker(sceneLoader);
    QObject::connect(sceneLoader, &Qt3DRender::QSceneLoader::statusChanged, &sceneWalker, &SceneWalker::onStatusChanged);
    sceneLoaderEntity->addComponent(sceneLoader);

    QStringList args = QCoreApplication::arguments();
    QUrl sourceFileName;
    if (args.count() <= 1) {
        QWidget *container = new QWidget();
        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::AnyFile);
        sourceFileName = dialog.getOpenFileUrl(container, QStringLiteral("Open a scene file"));
    } else {
        sourceFileName = QUrl::fromLocalFile(args[1]);
    }

    if (sourceFileName.isEmpty())
        return 0;

    sceneLoader->setSource(sourceFileName);

    view.setRootEntity(sceneRoot);
    view.show();

    return app.exec();
}
