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

#include "scenemodifier.h"

#include <QGuiApplication>

#include <window.h>
#include <Qt3DCore/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qcameralens.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtGui/QScreen>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRenderer/qtorusmesh.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qsceneloader.h>

#include <Qt3DCore/qscaletransform.h>
#include <Qt3DCore/qrotatetransform.h>
#include <Qt3DCore/qlookattransform.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>

#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qforwardrenderer.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Window *view = new Window();
    QWidget *container = QWidget::createWindowContainer(view);
    QSize screenSize = view->screen()->size();
    container->setMinimumSize(QSize(200, 100));
    container->setMaximumSize(screenSize);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);

    widget->setWindowTitle(QStringLiteral("Basic shapes"));

    Qt3D::QAspectEngine engine;
    engine.registerAspect(new Qt3D::QRenderAspect());
    Qt3D::QInputAspect *input = new Qt3D::QInputAspect;
    engine.registerAspect(input);
    engine.initialize();
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(view));
    engine.setData(data);

    // Root entity
    Qt3D::QEntity *rootEntity = new Qt3D::QEntity();

    // Camera
    Qt3D::QCamera *cameraEntity = new Qt3D::QCamera(rootEntity);
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, -20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    input->setCamera(cameraEntity);

    // FrameGraph
    Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
    Qt3D::QForwardRenderer *forwardRenderer = new Qt3D::QForwardRenderer();

    forwardRenderer->setCamera(cameraEntity);
    forwardRenderer->setClearColor(QColor(QRgb(0x4d4d4f)));
    frameGraph->setActiveFrameGraph(forwardRenderer);

    // Setting the FrameGraph
    rootEntity->addComponent(frameGraph);

    // Scenemodifier
    SceneModifier *modifier = new SceneModifier(rootEntity);

    // Set root object of the scene
    engine.setRootEntity(rootEntity);

    // Create control widgets
    QCommandLinkButton *info = new QCommandLinkButton();
    info->setText(QStringLiteral("Qt3D ready-made meshes"));
    info->setDescription(QStringLiteral("Qt3D provides several ready-made meshes, like torus, cylinder, cube and sphere."));
    info->setIconSize(QSize(0,0));

    QCheckBox *torusCB = new QCheckBox(widget);
    torusCB->setChecked(true);
    torusCB->setText(QStringLiteral("Torus"));

    QCheckBox *cylinderCB = new QCheckBox(widget);
    cylinderCB->setChecked(true);
    cylinderCB->setText(QStringLiteral("Cylinder"));

    QCheckBox *cuboidCB = new QCheckBox(widget);
    cuboidCB->setChecked(true);
    cuboidCB->setText(QStringLiteral("Cuboid"));

    QCheckBox *sphereCB = new QCheckBox(widget);
    sphereCB->setChecked(true);
    sphereCB->setText(QStringLiteral("Sphere"));

    vLayout->addWidget(info);
    vLayout->addWidget(torusCB);
    vLayout->addWidget(cylinderCB);
    vLayout->addWidget(cuboidCB);
    vLayout->addWidget(sphereCB);

    QObject::connect(torusCB, &QCheckBox::stateChanged,
                     modifier, &SceneModifier::enableTorus);
    QObject::connect(cylinderCB, &QCheckBox::stateChanged,
                     modifier, &SceneModifier::enableCylinder);
    QObject::connect(cuboidCB, &QCheckBox::stateChanged,
                     modifier, &SceneModifier::enableCuboid);
    QObject::connect(sphereCB, &QCheckBox::stateChanged,
                     modifier, &SceneModifier::enableSphere);

    torusCB->setChecked(true);
    cylinderCB->setChecked(true);
    cuboidCB->setChecked(true);
    sphereCB->setChecked(true);

    // Show window
    widget->show();
    widget->resize(1200, 800);

    // Update the aspect ratio
    QSize widgetSize =  container->size();
    float aspectRatio = float(widgetSize.width()) / float(widgetSize.height());
    cameraEntity->lens()->setPerspectiveProjection(45.0f, aspectRatio, 0.1f, 1000.0f);

    return app.exec();
}
