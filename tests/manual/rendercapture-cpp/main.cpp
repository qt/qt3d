// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QApplication>

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QPropertyAnimation>

#include "qt3dwindow.h"
#include "orbittransformcontroller.h"
#include "qorbitcameracontroller.h"
#include "mycapture.h"

Qt3DCore::QEntity *createScene()
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);

    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform;
    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);

    // Sphere
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(3);

    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;
    OrbitTransformController *controller = new OrbitTransformController(sphereTransform);
    controller->setTarget(sphereTransform);
    controller->setRadius(20.0f);

    QPropertyAnimation *sphereRotateTransformAnimation = new QPropertyAnimation(sphereTransform);
    sphereRotateTransformAnimation->setTargetObject(controller);
    sphereRotateTransformAnimation->setPropertyName("angle");
    sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
    sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
    sphereRotateTransformAnimation->setDuration(10000);
    sphereRotateTransformAnimation->setLoopCount(-1);
    sphereRotateTransformAnimation->start();

    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereTransform);
    sphereEntity->addComponent(material);

    return rootEntity;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    Qt3DCore::QEntity *scene = createScene();
    view.setRootEntity(scene);
    view.resize(600, 600);

    Qt3DRender::QRenderCapture* capture = new Qt3DRender::QRenderCapture;
    view.activeFrameGraph()->setParent(capture);
    view.setActiveFrameGraph(capture);

    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    QWidget *container = QWidget::createWindowContainer(&view);
    container->setMinimumSize(QSize(600, 600));
    container->setMaximumSize(QSize(600, 600));

    QPushButton *captureButton = new QPushButton();
    captureButton->setText("Capture");
    QCheckBox *checkBox = new QCheckBox();
    checkBox->setText("continuous");

    QLabel *imageLabel = new QLabel();
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->resize(600, 600);
    imageLabel->setMinimumSize(QSize(600, 600));
    imageLabel->setMaximumSize(QSize(600, 600));

    MyCapture myc(capture, imageLabel);
    QObject::connect(captureButton, &QPushButton::pressed, &myc, &MyCapture::capture);
    QObject::connect(checkBox, &QCheckBox::clicked, &myc, &MyCapture::setContinuous);

    // create widget
    QWidget *widget = new QWidget;
    widget->setWindowTitle(QStringLiteral("RenderCapture example"));

    QWidget* subWidget = new QWidget;
    QVBoxLayout *vLayout = new QVBoxLayout(subWidget);
    vLayout->addWidget(captureButton);
    vLayout->addWidget(checkBox);
    vLayout->addWidget(imageLabel);

    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    hLayout->addWidget(container);
    hLayout->addWidget(subWidget);

    // Show window
    widget->show();
    widget->resize(1250, 700);

    return app.exec();
}
