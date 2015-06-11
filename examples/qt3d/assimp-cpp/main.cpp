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
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QSceneLoader>
#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QForwardRenderer>


int main(int ac, char **av)
{
    QApplication app(ac, av);

    Window view;

    Qt3D::QAspectEngine engine;
    Qt3D::QInputAspect *inputAspect = new Qt3D::QInputAspect();
    engine.registerAspect(new Qt3D::QRenderAspect());
    engine.registerAspect(inputAspect);
    engine.initialize();
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.setData(data);

    // Root entity
    Qt3D::QEntity *sceneRoot = new Qt3D::QEntity();

    // Scene Camera
    Qt3D::QCamera *basicCamera = new Qt3D::QCamera(sceneRoot);
    basicCamera->setProjectionType(Qt3D::QCameraLens::PerspectiveProjection);
    basicCamera->setAspectRatio(view.width() / view.height());
    basicCamera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    basicCamera->setViewCenter(QVector3D(0.0f, 3.5f, 0.0f));
    basicCamera->setPosition(QVector3D(0.0f, 3.5f, 25.0f));
    basicCamera->setNearPlane(0.001f);
    basicCamera->setFarPlane(10000.0f);
    // For camera controls
    inputAspect->setCamera(basicCamera);

    // Forward Renderer FrameGraph
    Qt3D::QFrameGraph *frameGraphComponent = new Qt3D::QFrameGraph(sceneRoot);
    Qt3D::QForwardRenderer *forwardRenderer = new Qt3D::QForwardRenderer();
    forwardRenderer->setCamera(basicCamera);
    forwardRenderer->setClearColor(Qt::black);
    frameGraphComponent->setActiveFrameGraph(forwardRenderer);
    sceneRoot->addComponent(frameGraphComponent);

    // Scene loader
    Qt3D::QEntity *sceneLoaderEntity = new Qt3D::QEntity(sceneRoot);
    Qt3D::QSceneLoader *sceneLoader = new Qt3D::QSceneLoader(sceneLoaderEntity);
    sceneLoaderEntity->addComponent(sceneLoader);

    QWidget *container = new QWidget();
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    sceneLoader->setSource(dialog.getOpenFileUrl(container, QStringLiteral("Open a scene file")));

    engine.setRootEntity(sceneRoot);
    view.show();

    return app.exec();
}
