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

#include <QGuiApplication>

#include <window.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QCamera>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QPhongMaterial>
#include <Qt3DRender/QDiffuseMapMaterial>
#include <Qt3DRender/QForwardRenderer>
#include <Qt3DRender/QFrameGraph>
#include <Qt3DRender/QTextureImage>

#include "planeentity.h"
#include "rotatingtrefoilknot.h"
#include "barrel.h"
#include "houseplant.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Window view;
    Qt3DCore::QAspectEngine engine;
    engine.registerAspect(new Qt3DRender::QRenderAspect());
    Qt3DInput::QInputAspect *input = new Qt3DInput::QInputAspect;
    engine.registerAspect(input);
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("eventSource"), QVariant::fromValue(&view));
    engine.setData(data);

    // Scene Root
    Qt3DCore::QEntity *sceneRoot = new Qt3DCore::QEntity();

    // Scene Camera
    Qt3DCore::QCamera *basicCamera = new Qt3DCore::QCamera(sceneRoot);
    basicCamera->setProjectionType(Qt3DCore::QCameraLens::PerspectiveProjection);
    basicCamera->setAspectRatio(view.width() / view.height());
    basicCamera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    basicCamera->setViewCenter(QVector3D(0.0f, 3.5f, 0.0f));
    basicCamera->setPosition(QVector3D(0.0f, 3.5f, 25.0f));
    // For camera controls
    input->setCamera(basicCamera);

    // Forward Renderer FrameGraph
    Qt3DRender::QFrameGraph *frameGraphComponent = new Qt3DRender::QFrameGraph(sceneRoot);
    Qt3DRender::QForwardRenderer *forwardRenderer = new Qt3DRender::QForwardRenderer();
    forwardRenderer->setCamera(basicCamera);
    frameGraphComponent->setActiveFrameGraph(forwardRenderer);
    sceneRoot->addComponent(frameGraphComponent);

    // Scene floor
    PlaneEntity *planeEntity = new PlaneEntity(sceneRoot);
    planeEntity->mesh()->setHeight(100.0f);
    planeEntity->mesh()->setWidth(100.0f);
    planeEntity->mesh()->setMeshResolution(QSize(20, 20));

    Qt3DRender::QNormalDiffuseSpecularMapMaterial *normalDiffuseSpecularMapMaterial = new Qt3DRender::QNormalDiffuseSpecularMapMaterial();
    normalDiffuseSpecularMapMaterial->setTextureScale(10.0f);
    normalDiffuseSpecularMapMaterial->setShininess(80.0f);
    normalDiffuseSpecularMapMaterial->setAmbient(QColor::fromRgbF(0.2f, 0.2f, 0.2f, 1.0f));

    Qt3DRender::QTextureImage *diffuseImage = new Qt3DRender::QTextureImage();
    diffuseImage->setSource(QUrl(QStringLiteral("qrc:/assets/textures/pattern_09/diffuse.webp")));
    normalDiffuseSpecularMapMaterial->diffuse()->addTextureImage(diffuseImage);

    Qt3DRender::QTextureImage *specularImage = new Qt3DRender::QTextureImage();
    specularImage->setSource(QUrl(QStringLiteral("qrc:/assets/textures/pattern_09/specular.webp")));
    normalDiffuseSpecularMapMaterial->specular()->addTextureImage(specularImage);

    Qt3DRender::QTextureImage *normalImage = new Qt3DRender::QTextureImage();
    normalImage->setSource(QUrl((QStringLiteral("qrc:/assets/textures/pattern_09/normal.webp"))));
    normalDiffuseSpecularMapMaterial->normal()->addTextureImage(normalImage);

    planeEntity->addComponent(normalDiffuseSpecularMapMaterial);

    // Chest
    RenderableEntity *chest = new RenderableEntity(sceneRoot);
    chest->transform()->setScale(0.03f);
    chest->mesh()->setSource(QUrl(QStringLiteral("qrc:/assets/chest/Chest.obj")));
    Qt3DRender::QDiffuseMapMaterial *diffuseMapMaterial = new Qt3DRender::QDiffuseMapMaterial();
    diffuseMapMaterial->setSpecular(QColor::fromRgbF(0.2f, 0.2f, 0.2f, 1.0f));
    diffuseMapMaterial->setShininess(2.0f);

    Qt3DRender::QTextureImage *chestDiffuseImage = new Qt3DRender::QTextureImage();
    chestDiffuseImage->setSource(QUrl(QStringLiteral("qrc:/assets/chest/diffuse.webp")));
    diffuseMapMaterial->diffuse()->addTextureImage(chestDiffuseImage);

    chest->addComponent(diffuseMapMaterial);


    // TrefoilKnot
    RotatingTrefoilKnot *trefoil = new RotatingTrefoilKnot(sceneRoot);
    trefoil->setPosition(QVector3D(0.0f, 3.5f, 0.0f));
    trefoil->setScale(0.5f);
    Qt3DRender::QPhongMaterial *phongMaterial = new Qt3DRender::QPhongMaterial();
    phongMaterial->setDiffuse(QColor(204, 205, 75)); // Safari Yellow #cccd4b
    phongMaterial->setSpecular(Qt::white);
    trefoil->addComponent(phongMaterial);

    // Barrels
    Barrel *basicBarrel = new Barrel(sceneRoot);
    basicBarrel->transform()->setTranslation(QVector3D(8.0f, 0.0f, 0.0f));

    Barrel *rustyBarrel = new Barrel(sceneRoot);
    rustyBarrel->setDiffuse(Barrel::RustDiffuse);
    rustyBarrel->setSpecular(Barrel::RustSpecular);
    rustyBarrel->setBumps(Barrel::HardBumps);
    rustyBarrel->transform()->setTranslation(QVector3D(10.0f, 0.0f, 0.0f));

    Barrel *blueBarrel = new Barrel(sceneRoot);
    blueBarrel->setDiffuse(Barrel::Blue);
    blueBarrel->setBumps(Barrel::MiddleBumps);
    blueBarrel->transform()->setTranslation(QVector3D(12.0f, 0.0f, 0.0f));

    Barrel *greenBarrel = new Barrel(sceneRoot);
    greenBarrel->setDiffuse(Barrel::Green);
    greenBarrel->setBumps(Barrel::SoftBumps);
    greenBarrel->transform()->setTranslation(QVector3D(14.0f, 0.0f, 0.0f));

    Barrel *stainlessBarrel = new Barrel(sceneRoot);
    stainlessBarrel->setDiffuse(Barrel::StainlessSteelDiffuse);
    stainlessBarrel->setBumps(Barrel::NoBumps);
    stainlessBarrel->setSpecular(Barrel::StainlessSteelSpecular);
    stainlessBarrel->setShininess(150.0f);
    stainlessBarrel->transform()->setTranslation(QVector3D(16.0f, 0.0f, 0.0f));

    // Plants
    HousePlant *squareBamboo = new HousePlant(sceneRoot);
    squareBamboo->setPotShape(HousePlant::Square);
    squareBamboo->setPosition(QVector3D(4.0f, 0.0f, 0.0f));

    HousePlant *trianglePalm = new HousePlant(sceneRoot);
    trianglePalm->setPlantType(HousePlant::Palm);
    trianglePalm->setPotShape(HousePlant::Triangle);
    trianglePalm->setPosition(QVector3D(0.0f, 0.0f, 4.0f));

    HousePlant *spherePine = new HousePlant(sceneRoot);
    spherePine->setPlantType(HousePlant::Pine);
    spherePine->setPotShape(HousePlant::Sphere);
    spherePine->setPosition(QVector3D(-4.0f, 0.0f, 0.0f));

    HousePlant *crossSpikes = new HousePlant(sceneRoot);
    crossSpikes->setPlantType(HousePlant::Spikes);
    crossSpikes->setPosition(QVector3D(0.0f, 0.0f, -4.0f));

    HousePlant *crossPalm = new HousePlant(sceneRoot);
    crossPalm->setPlantType(HousePlant::Palm);
    crossPalm->setPosition(QVector3D(0.0f, 0.0f, -8.0f));
    crossPalm->setScale(0.05f);

    HousePlant *crossShrub = new HousePlant(sceneRoot);
    crossShrub->setPlantType(HousePlant::Shrub);
    crossShrub->setPosition(QVector3D(0.0f, 0.0f, 8.0f));
    crossShrub->setScale(0.05f);

    engine.setRootEntity(sceneRoot);
    view.show();

    return app.exec();
}
