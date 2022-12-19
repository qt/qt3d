// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QFontDatabase>
#include <Qt3DCore/Qt3DCore>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/Qt3DExtras>
#include <Qt3DExtras/QExtrudedTextMesh>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
    view->setTitle(QStringLiteral("3D Text CPP"));
    view->defaultFrameGraph()->setClearColor(QColor(210, 210, 220));

    auto *root = new Qt3DCore::QEntity();

    { // plane
        auto *plane = new Qt3DCore::QEntity(root);
        auto *planeMesh = new Qt3DExtras::QPlaneMesh();
        auto *planeTransform = new Qt3DCore::QTransform();
        auto *planeMaterial = new Qt3DExtras::QPhongMaterial(root);
        planeMesh->setWidth(10); planeMesh->setHeight(10);
        planeTransform->setTranslation(QVector3D(0, 0, 0));
        planeMaterial->setDiffuse(QColor(150, 150, 150));

        plane->addComponent(planeMaterial);
        plane->addComponent(planeMesh);
        plane->addComponent(planeTransform);
    }

    auto *textMaterial = new Qt3DExtras::QPhongMaterial(root);
    { // text
        int i = 0;
        const QStringList fonts = QFontDatabase::families();

        for (const QString &family : fonts)
        {
            auto *text = new Qt3DCore::QEntity(root);
            auto *textMesh = new Qt3DExtras::QExtrudedTextMesh();

            auto *textTransform = new Qt3DCore::QTransform();
            QFont font(family, 32, -1, false);
            textTransform->setTranslation(QVector3D(-2.45f, i * .5f, 0));
            textTransform->setScale(.2f);
            textMesh->setDepth(.45f);
            textMesh->setFont(font);
            textMesh->setText(family);
            textMaterial->setDiffuse(QColor(111, 150, 255));

            text->addComponent(textMaterial);
            text->addComponent(textMesh);
            text->addComponent(textTransform);

            i++;
        }
    }

    { // camera
        float aspect = static_cast<float>(view->screen()->size().width()) / view->screen()->size().height();
        Qt3DRender::QCamera *camera = view->camera();
        camera->lens()->setPerspectiveProjection(65.f, aspect, 0.1f, 100.f);
        camera->setPosition(QVector3D(0, 5, 3));
        camera->setViewCenter(QVector3D(0, 5, 0));

        auto *cameraController = new Qt3DExtras::QOrbitCameraController(root);
        cameraController->setCamera(camera);
    }

    view->setRootEntity(root);
    view->show();

    return a.exec();
}
