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

#include <QtCore/QDebug>

SceneModifier::SceneModifier(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity)
{

    // Torus shape data
    //! [0]
    m_torus = new Qt3DRender::QTorusMesh();
    m_torus->setRadius(1.0f);
    m_torus->setMinorRadius(0.4f);
    m_torus->setRings(100);
    m_torus->setSlices(20);
    //! [0]

    // TorusMesh Transform
    //! [1]
    Qt3DCore::QScaleTransform *torusScale = new Qt3DCore::QScaleTransform();
    Qt3DCore::QTranslateTransform *torusTranslation = new Qt3DCore::QTranslateTransform();
    Qt3DCore::QRotateTransform *torusRotation = new Qt3DCore::QRotateTransform();
    Qt3DCore::QTransform *torusTransforms = new Qt3DCore::QTransform();

    torusScale->setScale3D(QVector3D(2.0f, 2.0f, 2.0f));
    torusTranslation->setTranslation(QVector3D(1.7f, 1.7f, 0.0f));
    torusRotation->setAngleDeg(25.0f);
    torusRotation->setAxis(QVector3D(0, 1, 0));

    torusTransforms->addTransform(torusRotation);
    torusTransforms->addTransform(torusTranslation);
    torusTransforms->addTransform(torusScale);
    //! [1]

    //! [2]
    Qt3DRender::QPhongMaterial *torusMaterial = new Qt3DRender::QPhongMaterial();
    torusMaterial->setDiffuse(QColor(QRgb(0xbeb32b)));
    //! [2]

    // Torus
    //! [3]
    m_torusEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_torusEntity->addComponent(m_torus);
    m_torusEntity->addComponent(torusMaterial);
    m_torusEntity->addComponent(torusTransforms);
    //! [3]

    // Cylinder shape data
    Qt3DRender::QCylinderMesh *cylinder = new Qt3DRender::QCylinderMesh();
    cylinder->setRadius(1);
    cylinder->setLength(3);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    // CylinderMesh Transform
    Qt3DCore::QScaleTransform *cylinderScale = new Qt3DCore::QScaleTransform();
    Qt3DCore::QRotateTransform *cylinderRotation = new Qt3DCore::QRotateTransform();
    Qt3DCore::QTranslateTransform *cylinderTranslation = new Qt3DCore::QTranslateTransform();
    Qt3DCore::QTransform *cylinderTransforms = new Qt3DCore::QTransform();

    cylinderScale->setScale3D(QVector3D(1.5f, 1.5f, 1.5f));
    cylinderTranslation->setTranslation(QVector3D(-2.6f, 2.4f, -1.5));
    cylinderRotation->setAngleDeg(45.0f);
    cylinderRotation->setAxis(QVector3D(1, 0, 0));

    cylinderTransforms->addTransform(cylinderTranslation);
    cylinderTransforms->addTransform(cylinderRotation);
    cylinderTransforms->addTransform(cylinderScale);

    Qt3DRender::QPhongMaterial *cylinderMaterial = new Qt3DRender::QPhongMaterial();
    cylinderMaterial->setDiffuse(QColor(QRgb(0x928327)));

    // Cylinder
    m_cylinderEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_cylinderEntity->addComponent(cylinder);
    m_cylinderEntity->addComponent(cylinderMaterial);
    m_cylinderEntity->addComponent(cylinderTransforms);

    // Cuboid shape data
    Qt3DRender::QCuboidMesh *cuboid = new Qt3DRender::QCuboidMesh();

    // CuboidMesh Transform
    Qt3DCore::QScaleTransform *cuboidScale = new Qt3DCore::QScaleTransform();
    Qt3DCore::QTranslateTransform *cuboidTranslation = new Qt3DCore::QTranslateTransform();
    Qt3DCore::QTransform *cuboidTransforms = new Qt3DCore::QTransform();

    cuboidScale->setScale3D(QVector3D(4.0f, 4.0f, 4.0f));
    cuboidTranslation->setTranslation(QVector3D(0.9f, -0.9f, 0.0f));

    cuboidTransforms->addTransform(cuboidTranslation);
    cuboidTransforms->addTransform(cuboidScale);

    Qt3DRender::QPhongMaterial *cuboidMaterial = new Qt3DRender::QPhongMaterial();
    cuboidMaterial->setDiffuse(QColor(QRgb(0x665423)));

    //Cuboid
    m_cuboidEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_cuboidEntity->addComponent(cuboid);
    m_cuboidEntity->addComponent(cuboidMaterial);
    m_cuboidEntity->addComponent(cuboidTransforms);

    // Sphere shape data
    Qt3DRender::QSphereMesh *sphereMesh = new Qt3DRender::QSphereMesh();
    sphereMesh->setRings(20);
    sphereMesh->setSlices(20);
    sphereMesh->setRadius(2);

    // Sphere mesh transform
    Qt3DCore::QScaleTransform *sphereScale = new Qt3DCore::QScaleTransform();
    Qt3DCore::QTranslateTransform *sphereTranslation = new Qt3DCore::QTranslateTransform();
    Qt3DCore::QTransform *sphereTransforms = new Qt3DCore::QTransform();

    sphereScale->setScale3D(QVector3D(1.3f, 1.3f, 1.3f));
    sphereTranslation->setTranslation(QVector3D(-2.9f, -2.9f, 0.0f));

    sphereTransforms->addTransform(sphereTranslation);
    sphereTransforms->addTransform(sphereScale);

    Qt3DRender::QPhongMaterial *sphereMaterial = new Qt3DRender::QPhongMaterial();
    sphereMaterial->setDiffuse(QColor(QRgb(0xa69929)));

    // Sphere
    m_sphereEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_sphereEntity->addComponent(sphereMesh);
    m_sphereEntity->addComponent(sphereMaterial);
    m_sphereEntity->addComponent(sphereTransforms);
}

SceneModifier::~SceneModifier()
{
}

//! [4]
void SceneModifier::enableTorus(bool enabled)
{
    m_torusEntity->setParent(enabled ? m_rootEntity : Q_NULLPTR);
}
//! [4]

void SceneModifier::enableCylinder(bool enabled)
{
    m_cylinderEntity->setParent(enabled ? m_rootEntity : Q_NULLPTR);
}

void SceneModifier::enableCuboid(bool enabled)
{
    m_cuboidEntity->setParent(enabled ? m_rootEntity : Q_NULLPTR);
}

void SceneModifier::enableSphere(bool enabled)
{
    m_sphereEntity->setParent(enabled ? m_rootEntity : Q_NULLPTR);
}
