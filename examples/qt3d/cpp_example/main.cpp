/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/qtorusmesh.h>
#include <Qt3DRender/qmesh.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qcameraselector.h>
#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/qframegraph.h>
#include <Qt3DRender/qclearbuffer.h>
#include "qt3dwindow.h"
#include "qfirstpersoncameracontroller.h"

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    Qt3DWindow view;

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    rootEntity->setObjectName(QStringLiteral("rootEntity"));
    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);

    // Torus shape data
    Qt3DRender::QTorusMesh *torus = new Qt3DRender::QTorusMesh();
    torus->setRadius(40);
    torus->setMinorRadius(15);

    torusEntity->addComponent(torus);

    // TorusMesh Transform
    Qt3DCore::QTransform *torusTransforms = new Qt3DCore::QTransform();
    torusTransforms->setTranslation(QVector3D(-5.0f, 3.5f, 2.0f));
    torusTransforms->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 35.0f));
    torusEntity->addComponent(torusTransforms);

    // Scene file
    Qt3DCore::QEntity *sceneEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QSceneLoader *scene = new Qt3DRender::QSceneLoader();
    scene->setObjectName(QStringLiteral("scene"));
    Qt3DCore::QTransform *sceneTransform = new Qt3DCore::QTransform();
    sceneTransform->setTranslation(QVector3D(2.5f, 0.5f, -10.0f));
    sceneEntity->addComponent(sceneTransform);
    scene->setSource(QUrl("qrc:/assets/test_scene.dae"));
    sceneEntity->addComponent(scene);

    // Camera
    Qt3DRender::QCamera *cameraEntity = view.camera();
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
    cameraEntity->lens()->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(5, 0, 20.0f));
    cameraEntity->setViewCenter(QVector3D(11, 0, 5));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));

    // For camera controls
    Qt3DInput::QFirstPersonCameraController *camController = new Qt3DInput::QFirstPersonCameraController(rootEntity);
    camController->setCamera(cameraEntity);

    // Set root object of the scene
    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
