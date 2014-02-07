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


#include "cylinderview.h"
#include "qglbuilder.h"
#include "qglcylinder.h"
#include <QtCore/qurl.h>

CylinderView::CylinderView(QWindow *parent)
    : QGLView(parent)
{
    QGLBuilder builder;
    builder << QGL::Smooth << QGLCylinder(2.0, 1.5, 2.0, 36, 3, true, true);

    QGLMaterial *matLid = new QGLMaterial;
    matLid->setAmbientColor(Qt::gray);
    matLid->setDiffuseColor(Qt::gray);
    QUrl urlLid;
    urlLid.setPath(QLatin1String(":/latte.png"));
    urlLid.setScheme(QLatin1String("file"));
    matLid->setTextureUrl(urlLid);
    m_LoadedTextures.push_back(matLid->texture());

    QGLMaterial *matSides = new QGLMaterial;
    matSides->setColor(QColor(170, 202, 0));
    QUrl urlSides;
    urlSides.setPath(QLatin1String(":/cupTexture.png"));
    urlSides.setScheme(QLatin1String("file"));
    matSides->setTextureUrl(urlSides);
    m_LoadedTextures.push_back(matSides->texture());

    QGLSceneNode *root = builder.sceneNode();

    QGLSceneNode *lid = root->findChild<QGLSceneNode *>("Cylinder Top");
    int lidMat = root->palette()->addMaterial(matLid);
    lid->setMaterialIndex(lidMat);
    lid->setEffect(QGL::LitDecalTexture2D);

    QGLSceneNode *sides = root->findChild<QGLSceneNode *>("Cylinder Sides");
    int sideMat = root->palette()->addMaterial(matSides);
    sides->setMaterialIndex(sideMat);
    sides->setEffect(QGL::LitDecalTexture2D);


    cylinder = builder.finalizedSceneNode();

    QGLMaterial *mat = new QGLMaterial;
    mat->setAmbientColor(Qt::gray);
    mat->setDiffuseColor(Qt::gray);
    cylinder->setMaterial(mat);
    cylinder->setEffect(QGL::LitMaterial);
}

CylinderView::~CylinderView()
{
    for (int i=0; i<m_LoadedTextures.count(); ++i) {
        m_LoadedTextures.at(i)->cleanupResources();
    }
    delete cylinder;
}

void CylinderView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().translate(0.0f, -1.0f, 0.0f);
    painter->modelViewMatrix().rotate(-75.0f, 1.0f, 0.0f, 0.0f);
    cylinder->draw(painter);
}

