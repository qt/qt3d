/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
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

#include "solarsystem.h"
#include "qglbuilder.h"
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
#include "qglmaterial.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"
#include "qglsphere.h"
#include "qgraphicsrotation3d.h"
#include "qgraphicstranslation3d.h"
#include "qglshaderprogrameffect.h"

#include <QPropertyAnimation>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <qmath.h>

SolarSystemView::SolarSystemView(QWindow *parent)
    : QGLView(parent)
    , spaceScene(new QGLSceneNode(this))
    , sunEffect(0)
{
    //Generate geometry for the scene
    spaceScene = createScene();

    //Set up the camera
    camera()->setEye(QVector3D(0, 0, 15));

    //Create the animation for the rotation of the Sun
    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(this, "angle1", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(7000);
    animation->setLoopCount(-1);
    animation->start();

    //Create the animation for the rotation of the Planet
    QPropertyAnimation *animation2;
    animation2 = new QPropertyAnimation(this, "angle2", this);
    animation2->setStartValue(0.0f);
    animation2->setEndValue(360.0f);
    animation2->setDuration(3000);
    animation2->setLoopCount(-1);
    animation2->start();

    //Create the animation for the rotation of the Solar-System
    QPropertyAnimation *animation3;
    animation3 = new QPropertyAnimation(this, "angle3", this);
    animation3->setStartValue(0.0f);
    animation3->setEndValue(360.0f);
    animation3->setDuration(30000);
    animation3->setLoopCount(-1);
    animation3->start();

    //Create the animation for the glow effect
    QPropertyAnimation *glowAnimation;
    glowAnimation = new QPropertyAnimation(this, "glowFactor", this);
    glowAnimation->setStartValue(0.0f);
    glowAnimation->setEndValue(0.0f);
    glowAnimation->setKeyValueAt(0.5, 1.0f);
    glowAnimation->setDuration(3000);
    glowAnimation->setLoopCount(-1);
    glowAnimation->start();
}

SolarSystemView::~SolarSystemView()
{    
    for (int i=0; i<m_LoadedTextures.count(); ++i) {
        m_LoadedTextures.at(i)->cleanupResources();
    }
    delete spaceScene;
    delete sunEffect;
}

void SolarSystemView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
    //nothing for now
}


void SolarSystemView::paintGL(QGLPainter *painter)
{
    sunRotation->setAngle(-m_angle1);
    planetRotation->setAngle(m_angle2);
    systemRotation->setAngle(m_angle3);
    // Set the effect active to make sure that the program is created
    // and bound so that we can update our uniform
    sunEffect->setActive(painter,true);
    sunEffect->program()->setUniformValue("glowFactor", m_glowFactor);
    spaceScene->draw(painter);
}

QGLSceneNode *SolarSystemView::createScene()
{ 
    QGLBuilder builder;
    QGLSceneNode *root = builder.sceneNode();
    QUrl url;

    //set up our materials palette - this describes all
    //of the materials we will use for this scene.

    //solar surface
    QGLMaterial *mat1 = new QGLMaterial;
    url.setPath(QLatin1String(":/solar.jpg"));
    url.setScheme(QLatin1String("file"));
    mat1->setTextureUrl(url,0);
    url.setPath(QLatin1String(":/solar2.jpg"));
    url.setScheme(QLatin1String("file"));
    mat1->setTextureUrl(url, 1);
    m_LoadedTextures.push_back(mat1->texture(0));
    m_LoadedTextures.push_back(mat1->texture(1));
    //mat1->setEmittedLight(Qt::white);
    int sunMat = root->palette()->addMaterial(mat1);

    //planetary surface
    QGLMaterial *mat2 = new QGLMaterial;
    url.setPath(QLatin1String(":/planet.jpg"));
    url.setScheme(QLatin1String("file"));
    mat2->setTextureUrl(url);
    m_LoadedTextures.push_back(mat2->texture());
    int planetMat = root->palette()->addMaterial(mat2);

    //moon surface
    QGLMaterial *mat3 = new QGLMaterial;
    url.setPath(QLatin1String(":/moon-texture.jpg"));
    url.setScheme(QLatin1String("file"));
    mat3->setTextureUrl(url);
    m_LoadedTextures.push_back(mat3->texture());
    int moonMat = root->palette()->addMaterial(mat3);

    //Set the rotation of the solar-system
    systemRotation = new QGraphicsRotation3D();
    systemRotation->setAxis(QVector3D(0,1,0));
    root->addTransform(systemRotation);

    // create the sun for our solar system
    builder.pushNode()->setObjectName(QLatin1String("Solar"));
    builder<<QGLSphere(1.5);
    builder.currentNode()->setMaterialIndex(sunMat);
    sunEffect = new QGLShaderProgramEffect();
    sunEffect->setVertexShaderFromFile(":/solar.vsh");
    sunEffect->setFragmentShaderFromFile(":/solar.fsh");
//    builder.currentNode()->setEffect(QGL::LitModulateTexture2D);
    builder.currentNode()->setUserEffect(sunEffect);

    //create and add rotations for axial tilt and rotation
    sunRotation = new QGraphicsRotation3D();
    QGraphicsRotation3D *axialTilt1 = new QGraphicsRotation3D();
    axialTilt1->setAngle(90.0f);
    axialTilt1->setAxis(QVector3D(1,0,0));
    builder.currentNode()->addTransform(sunRotation);
    builder.currentNode()->addTransform(axialTilt1);

    // create the planet
    builder.newNode()->setObjectName(QLatin1String("Planet"));
    builder<<QGLSphere(0.4);
    builder.currentNode()->setMaterialIndex(planetMat);
    builder.currentNode()->setEffect(QGL::LitModulateTexture2D);

    //position our planet - positioning occurs only AFTER transformations
    //have taken place.
    builder.currentNode()->setPosition(QVector3D(4.5,0,0));

    //create and add rotations for axial tilt and rotation
    planetRotation = new QGraphicsRotation3D();
    QGraphicsRotation3D *axialTilt2 = new QGraphicsRotation3D();
    axialTilt2->setAngle(75.0f);
    axialTilt2->setAxis(QVector3D(1,0,0));

    builder.currentNode()->addTransform(planetRotation);
    builder.currentNode()->addTransform(axialTilt2);

    //create the moon
    builder.pushNode()->setObjectName("Moon");
    builder<<QGLSphere(0.1);
    builder.currentNode()->setMaterialIndex(moonMat);
    builder.currentNode()->setEffect(QGL::LitModulateTexture2D);

    //as our moon is tidally locked to the planet (ie. they
    //always face the same side to one another), we are just
    //correcting the axial tilt of the moon here.
    QGraphicsRotation3D *axialTilt3 = new QGraphicsRotation3D();
    axialTilt3->setAngle(90.0f);
    axialTilt3->setAxis(QVector3D(1,0,0));
    builder.currentNode()->addTransform(axialTilt3);

    //The position is set relative to the parent node...
    builder.currentNode()->setPosition(QVector3D(0.7,0,0));





    //completed building, so finalise
    return builder.finalizedSceneNode();    
}
