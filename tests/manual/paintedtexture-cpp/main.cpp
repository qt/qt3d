// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "scene.h"

#include <QGuiApplication>

#include <Qt3DCore/qentity.h>
#include <QtGui/QScreen>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include <Qt3DRender/qcamera.h>
#include <Qt3DInput/QInputAspect>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
    view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    QWidget *container = QWidget::createWindowContainer(view);
    QSize screenSize = view->screen()->size();
    container->setMinimumSize(QSize(200, 100));
    container->setMaximumSize(screenSize);

    Qt3DInput::QInputAspect *input = new Qt3DInput::QInputAspect;
    view->registerAspect(input);

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *cameraEntity = view->camera();

    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QFirstPersonCameraController *camController = new Qt3DExtras::QFirstPersonCameraController(rootEntity);
    camController->setCamera(cameraEntity);

    // scene
    Scene *scene = new Scene(rootEntity);

    // Set root object of the scene
    view->setRootEntity(rootEntity);

    // 'Generate New' Button
    QPushButton *genNewButton = new QPushButton();
    genNewButton->setText("Generate New");
    QObject::connect(genNewButton, &QPushButton::pressed, scene, &Scene::redrawTexture);

    // current size label
    QLabel *sizeLabel = new QLabel();
    sizeLabel->setText(QString("Current Size: 128x128"));

    // 'Change Size' Slider
    QSlider *changeSizeSlider = new QSlider();
    changeSizeSlider->setOrientation(Qt::Horizontal);
    changeSizeSlider->setMinimum(0);
    changeSizeSlider->setMaximum(4);
    QObject::connect(changeSizeSlider, &QSlider::valueChanged, [scene, sizeLabel](int value) {
        int sz = 128 << value;
        scene->setSize(QSize(sz, sz));
        sizeLabel->setText(QString("Current Size: %1x%1").arg(sz));
    });

    // create widget
    QWidget *widget = new QWidget;
    widget->setWindowTitle(QStringLiteral("QPaintedTexture Example"));

    QVBoxLayout *hLayout = new QVBoxLayout(widget);
    hLayout->addWidget(container, 1);
    hLayout->addWidget(genNewButton, 1);
    hLayout->addWidget(changeSizeSlider, 1);
    hLayout->addWidget(sizeLabel);

    // Show window
    widget->show();
    widget->resize(1200, 800);

    return app.exec();
}
