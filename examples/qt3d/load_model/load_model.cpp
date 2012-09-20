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

#include "load_model.h"
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
#include "qglmaterial.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"
#include "qglabstractscene.h"

#include <QtGui/qmatrix4x4.h>

#include <QPropertyAnimation>
#include <QtCore/qmath.h>

#define DEGREE_TO_RAD (3.1415926/180.0)

LoadModelView::LoadModelView(QWindow *parent)
    : QGLView(parent)
    , m_pTeapotScene(0)
    , m_pTeapotTexture(0)
    , m_AngleTeapot(0)
    , m_AngleTeapotOrbit(0)
    , m_pLanderScene(0)
    , m_pLanderTexture(0)
    , m_AngleLander(0)
{
    loadModels();

    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "angleTeapot", this);
        animation->setStartValue(0.0f);
        animation->setEndValue(360.0f);
        animation->setDuration(1000);
        animation->setLoopCount(-1);
        animation->start();
    }
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "angleTeapotOrbit", this);
        animation->setStartValue(0.0f);
        animation->setEndValue(360.0f);
        animation->setDuration(8000);
        animation->setLoopCount(-1);
        animation->start();
    }
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "angleLander", this);
        animation->setStartValue(0.0f);
        animation->setEndValue(360.0f);
        animation->setDuration(2000);
        animation->setLoopCount(-1);
        animation->start();
    }

    camera()->setCenter(QVector3D(0, 0, 0));
    camera()->setEye(QVector3D(0, 4, 10));
}

LoadModelView::~LoadModelView()
{
    m_pTeapotTexture->cleanupResources();
    m_pLanderTexture->cleanupResources();
    delete m_pTeapotScene;
    delete m_pLanderScene;
}

void LoadModelView::paintGL(QGLPainter *painter)
{
    QMatrix4x4 landerWorld;
    landerWorld.setToIdentity();
    landerWorld.scale(0.2);
    landerWorld.translate(QVector3D(0.0,sin(DEGREE_TO_RAD*angleLander()),0.0));
    painter->modelViewMatrix() = camera()->modelViewMatrix() * landerWorld;
    m_pLanderScene->mainNode()->draw(painter);

    QMatrix4x4 teapotWorld;
    teapotWorld.setToIdentity();
    teapotWorld.scale(1.0);
    teapotWorld.rotate(angleTeapotOrbit(),0,1,0);
    teapotWorld.translate(QVector3D(2.0,0.0,0.0));
    teapotWorld.rotate(angleTeapot(),0,1,0);
    painter->modelViewMatrix() = camera()->modelViewMatrix() * teapotWorld;
    m_pTeapotScene->mainNode()->draw(painter);
}

void FixNodesRecursive(int matIndex, QGLSceneNode* pNode)
{
    if (pNode) {
        pNode->setMaterialIndex(matIndex);
        pNode->setEffect(QGL::FlatReplaceTexture2D);
        foreach (QGLSceneNode* pCh, pNode->children()) {
            FixNodesRecursive(matIndex, pCh);
        }
    }
}

void LoadModelView::loadModels()
{
    {
        m_pTeapotScene = QGLAbstractScene::loadScene(QLatin1String(":/models/teapot.bez"));
        Q_ASSERT(m_pTeapotScene!=0);
        QGLMaterial *mat = new QGLMaterial;
        mat->setAmbientColor(Qt::lightGray);
        mat->setDiffuseColor(Qt::lightGray);
        QUrl url;
        url.setPath(QLatin1String(":/images/qt-soup.png"));
        url.setScheme(QLatin1String("file"));
        mat->setTextureUrl(url);
        m_pTeapotTexture = mat->texture();
        Q_ASSERT(m_pTeapotTexture!=0);
        QGLSceneNode* pTeapotSceneRoot = m_pTeapotScene->mainNode();
        int matIndex = pTeapotSceneRoot->palette()->addMaterial(mat);
        pTeapotSceneRoot->setMaterialIndex(matIndex);
        pTeapotSceneRoot->setEffect(QGL::FlatReplaceTexture2D);
        FixNodesRecursive(matIndex,pTeapotSceneRoot);
    }

    {
        m_pLanderScene = QGLAbstractScene::loadScene(QLatin1String(":/models/lunar-lander.obj"));
        Q_ASSERT(m_pLanderScene!=0);
        QGLMaterial *mat = new QGLMaterial;
        mat->setAmbientColor(Qt::lightGray);
        mat->setDiffuseColor(Qt::lightGray);
        QUrl url;
        url.setPath(QLatin1String(":/images/rusty.png"));
        url.setScheme(QLatin1String("file"));
        mat->setTextureUrl(url);
        m_pLanderTexture = mat->texture();
        Q_ASSERT(m_pLanderTexture!=0);
        QGLSceneNode* pLanderSceneRoot = m_pLanderScene->mainNode();
        int matIndex = pLanderSceneRoot->palette()->addMaterial(mat);
        pLanderSceneRoot->setMaterialIndex(matIndex);
        pLanderSceneRoot->setEffect(QGL::FlatReplaceTexture2D);
        FixNodesRecursive(matIndex,pLanderSceneRoot);
    }
}
