// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>

#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DExtras/qcylindermesh.h>
#include <Qt3DExtras/qspheremesh.h>
#include <Qt3DExtras/qphongmaterial.h>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qorbitcameracontroller.h>

#include <QtGui/qquaternion.h>

#include <QTimer>

class ComponentSwapper : public QObject
{
    Q_OBJECT
public:
    ComponentSwapper(Qt3DCore::QEntity *entity,
                     Qt3DCore::QComponent *component1,
                     Qt3DCore::QComponent *component2,
                     QObject * parent = nullptr)
        : QObject(parent)
        , m_entity(entity)
        , m_component1(component1)
        , m_component2(component2)
        , m_currentComponent(component1)
    {
        // Set initial state
        m_entity->addComponent(m_component1);
    }

public slots:
    void swapComponents()
    {
        if (m_currentComponent == m_component1) {
            m_entity->removeComponent(m_component1);
            m_entity->addComponent(m_component2);
            m_currentComponent = m_component2;
        } else {
            m_entity->removeComponent(m_component2);
            m_entity->addComponent(m_component1);
            m_currentComponent = m_component1;
        }
    }

private:
    Qt3DCore::QEntity *m_entity;
    Qt3DCore::QComponent *m_component1;
    Qt3DCore::QComponent *m_component2;
    Qt3DCore::QComponent *m_currentComponent;
};


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->setPosition(QVector3D(0, 0, 20.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *cameraController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    cameraController->setCamera(camera);

    // Cylinder mesh data
    Qt3DExtras::QCylinderMesh *cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius(1);
    cylinderMesh->setLength(3);
    cylinderMesh->setRings(5);
    cylinderMesh->setSlices(40);

    // Sphere mesh data
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(20);
    sphereMesh->setSlices(40);

    // Transform for cylinder
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
    transform->setScale(1.5f);
    transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    // Material
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setDiffuse(Qt::red);

    // Cylinder
    Qt3DCore::QEntity *cylinder = new Qt3DCore::QEntity(rootEntity);
    cylinder->addComponent(transform);
    cylinder->addComponent(material);

    // Set root object of the scene
    view.setRootEntity(rootEntity);
    view.show();

    ComponentSwapper *swapper = new ComponentSwapper(cylinder, cylinderMesh, sphereMesh);
    QTimer *timer = new QTimer;
    QObject::connect(timer, SIGNAL(timeout()), swapper, SLOT(swapComponents()));
    timer->start(2000);

    return app.exec();
}

#include "main.moc"
