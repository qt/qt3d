/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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

#include "scene_animations.h"
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
#include "qglmaterial.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"
#include "qglabstractscene.h"

#include <QtGui/qmatrix4x4.h>

#include <QPropertyAnimation>
#include <QtCore/qmath.h>
#include <QTimer>

AnimationsView::AnimationsView(QWindow *parent)
    : QGLView(parent)
    , m_pScene(0)
    , m_pTexture(0)
    , m_bPaused(false)
{
    loadAssets();
    QObject::connect(m_pScene,SIGNAL(animationUpdated()),this,SLOT(animationUpdated()));
    QList<QGLSceneAnimation *> anims = m_pScene->animations();
    for (int i=0; i<anims.size(); ++i) {
        QGLSceneAnimation* pAnim = anims.at(i);
        pAnim->play();
    }

    QTimer::singleShot(500+(rand()%1500),this,SLOT(timerEvent()));

    camera()->setCenter(QVector3D(0, 2, 0));
    camera()->setEye(QVector3D(12, 24, 40));
}

AnimationsView::~AnimationsView()
{
    m_pTexture->cleanupResources();
    delete m_pScene;
}

void AnimationsView::animationUpdated()
{
    update();
}

void AnimationsView::timerEvent()
{
    QList<QGLSceneAnimation *> anims = m_pScene->animations();
    m_bPaused = !m_bPaused;
    if (m_bPaused) {
        for (int i=0; i<anims.size(); ++i) {
            QGLSceneAnimation* pAnim = anims.at(i);
            pAnim->pause(true);
        }
    } else {
        for (int i=0; i<anims.size(); ++i) {
            QGLSceneAnimation* pAnim = anims.at(i);
            pAnim->pause(false);
        }
    }
    QTimer::singleShot(500+(rand()%1500),this,SLOT(timerEvent()));
}

void AnimationsView::paintGL(QGLPainter *painter)
{
    m_pScene->mainNode()->draw(painter);
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

void AnimationsView::loadAssets()
{
    {
        m_pScene = QGLAbstractScene::loadScene(QLatin1String(":/models/scale_rotate_translate.dae"));
        Q_ASSERT(m_pScene!=0);
        QGLMaterial *mat = new QGLMaterial;
        mat->setAmbientColor(Qt::lightGray);
        mat->setDiffuseColor(Qt::lightGray);
        QUrl url;
        url.setPath(QLatin1String(":/images/stonewal.jpg"));
        url.setScheme(QLatin1String("file"));
        mat->setTextureUrl(url);
        m_pTexture = mat->texture();
        Q_ASSERT(m_pTexture!=0);
        QGLSceneNode* pSceneRoot = m_pScene->mainNode();
        int matIndex = pSceneRoot->palette()->addMaterial(mat);
        pSceneRoot->setMaterialIndex(matIndex);
        pSceneRoot->setEffect(QGL::FlatReplaceTexture2D);
        FixNodesRecursive(matIndex,pSceneRoot);
    }
}
