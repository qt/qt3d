/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QtQml>

#include <exampleresources.h>

#include <window.h>
#include <scene.h>
#include <entity.h>
#include <camera.h>
#include <shape.h>
#include <mesh.h>
#include <technique.h>
#include <material.h>
#include <effect.h>
#include <translatetransform.h>
#include <matrixtransform.h>
#include <rotatetransform.h>
#include <texture.h>
#include <renderpass.h>
#include <renderpassfilter.h>
#include <techniquefilter.h>
#include <viewport.h>
#include <cameraselector.h>

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    initializeAssetResources("../exampleresources/example-assets.qrb");

    qmlRegisterType<Qt3D::Node>("Qt3D", 2, 0, "Node");
    qmlRegisterType<Qt3D::Entity>("Qt3D", 2, 0, "Entity");

    qmlRegisterType<Qt3D::MatrixTransform>("Qt3D", 2, 0, "MatrixTransform");

    Qt3D::Window view;

    Qt3D::Entity *rootEntity = new Qt3D::Entity();
    Qt3D::Scene  *scene = new Qt3D::Scene();

    Qt3D::Entity *torusEntity = new Qt3D::Entity();
    Qt3D::Mesh *torusMesh = new Qt3D::Mesh();
    Qt3D::Shape *torus = new Qt3D::Shape();

    torus->setType(Qt3D::Shape::ShapeTorus);
    torus->setRadius(40);
    torus->setMinorRadius(15);

    torusMesh->setData(torus->data());
    torusEntity->addComponent(torusMesh);

    Qt3D::TranslateTransform *torusTranslation = new Qt3D::TranslateTransform();
    Qt3D::RotateTransform *torusRotation = new Qt3D::RotateTransform();

    torusTranslation->setTranslation(QVector3D(2.5f, 3.5f, 2.0f));
    torusRotation->setAxis(QVector3D(1, 0, 0));
    torusRotation->setAngleDeg(35.0f);

    torusEntity->appendTransfrom(torusTranslation);
    torusEntity->appendTransfrom(torusRotation);

    scene->setSource(":/assets/gltf/wine/wine.json");

    // Build Node Tree
    scene->addChild(torusEntity);
    rootEntity->addChild(scene);

    // Set root object of the scene
    view.setRootObject(rootEntity);

    // Show window
    view.show();

    return app.exec();
}
