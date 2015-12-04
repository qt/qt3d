/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QFileDialog>
#include <QApplication>

#include <window.h>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DInput/QInputAspect>
#include <Qt3DRender/QFrameGraph>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QForwardRenderer>

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
    if (m_loader->status() != Qt3DRender::QSceneLoader::Loaded)
        return;

    // The QSceneLoader instance is a component of an entity. The loaded scene
    // tree is added under this entity.
    QVector<Qt3DCore::QEntity *> entities = m_loader->entities();

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
    Qt3DCore::QNodeList nodes = e->childrenNodes();
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

    Window view;

    Qt3DCore::QAspectEngine engine;
    Qt3DInput::QInputAspect *inputAspect = new Qt3DInput::QInputAspect();
    engine.registerAspect(new Qt3DRender::QRenderAspect());
    engine.registerAspect(inputAspect);
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.setData(data);

    // Root entity
    Qt3DCore::QEntity *sceneRoot = new Qt3DCore::QEntity();

    // Scene Camera
    Qt3DCore::QCamera *basicCamera = new Qt3DCore::QCamera(sceneRoot);
    basicCamera->setProjectionType(Qt3DCore::QCameraLens::PerspectiveProjection);
    basicCamera->setAspectRatio(view.width() / view.height());
    basicCamera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    basicCamera->setViewCenter(QVector3D(0.0f, 3.5f, 0.0f));
    basicCamera->setPosition(QVector3D(0.0f, 3.5f, 25.0f));
    basicCamera->setNearPlane(0.001f);
    basicCamera->setFarPlane(10000.0f);
    // For camera controls
    inputAspect->setCamera(basicCamera);

    // Forward Renderer FrameGraph
    Qt3DRender::QFrameGraph *frameGraphComponent = new Qt3DRender::QFrameGraph(sceneRoot);
    Qt3DRender::QForwardRenderer *forwardRenderer = new Qt3DRender::QForwardRenderer();
    forwardRenderer->setCamera(basicCamera);
    forwardRenderer->setClearColor(Qt::black);
    frameGraphComponent->setActiveFrameGraph(forwardRenderer);
    sceneRoot->addComponent(frameGraphComponent);

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

    engine.setRootEntity(sceneRoot);
    view.show();

    return app.exec();
}
