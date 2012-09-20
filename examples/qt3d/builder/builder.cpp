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

#include "builder.h"
#include "qglbuilder.h"
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
#include "qglmaterial.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"

#include <QtGui/qmatrix4x4.h>

#include <QtCore/qmath.h>

BuilderView::BuilderView(QWindow *parent)
    : QGLView(parent)
    , canScene(new QGLSceneNode(this))
    , texture(0)
{
    //! [0]
    QGLSceneNode *can = buildGeometry();
    canScene->addNode(can);
    {
        // rotate the can around so its label shows; and down
        // so the base is facing down
        QMatrix4x4 mat;
        QQuaternion q1 = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 270.0f);
        QQuaternion q2 = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 100.0f);
        mat.rotate(q2 * q1);
        can->setLocalTransform(mat);
    }

    // display a copy of the can to the left
    QGLSceneNode *node = new QGLSceneNode(canScene);
    node->addNode(can);
    {
        QMatrix4x4 mat;
        mat.translate(-2.0f, 0.0f, -2.0f);
        node->setLocalTransform(mat);
    }

    // display a copy of the can to the right
    node = new QGLSceneNode(canScene);
    node->addNode(can);
    {
        QMatrix4x4 mat;
        mat.translate(2.0f, 0.0f, -2.0f);
        node->setLocalTransform(mat);
    }
    //! [0]

    // rotate the whole scene about x-axis so that
    // can tops are visible when scene is first displayed
    {
        QMatrix4x4 mat;
        mat.rotate(1.0f, 0.0f, 0.0f, -30.0f);
        canScene->setLocalTransform(mat);
    }
}

BuilderView::~BuilderView()
{
    texture->cleanupResources();
    delete canScene;
}

void BuilderView::initializeGL(QGLPainter *painter)
{
    QGLLightParameters *light0 = new QGLLightParameters(this);
    light0->setAmbientColor(Qt::white);
    light0->setDiffuseColor(Qt::white);
    light0->setDirection(QVector3D(0.0f, 0.2f, 2.0f));
    painter->setMainLight(light0);
    QGLLightModel *model = new QGLLightModel(this);
    model->setAmbientSceneColor(Qt::white);
    painter->setLightModel(model);
}

//! [1]
void BuilderView::paintGL(QGLPainter *painter)
{
    canScene->draw(painter);
}
//! [1]

QGLSceneNode *BuilderView::buildGeometry()
{
    //! [2]
    QGLBuilder builder;
    QGLSceneNode *root = builder.sceneNode();

    QGLMaterial *mat = new QGLMaterial;
    mat->setAmbientColor(Qt::lightGray);
    mat->setDiffuseColor(Qt::lightGray);
    QUrl url;
    url.setPath(QLatin1String(":/images/qt-soup.png"));
    url.setScheme(QLatin1String("file"));
    mat->setTextureUrl(url);
    texture = mat->texture();
    int canMat = root->palette()->addMaterial(mat);
    root->setMaterialIndex(canMat);
    root->setEffect(QGL::LitMaterial);
    //! [2]

    // size data for can
    const float canRadius = 1.0f;
    const float canHeight = 2.5f;
    const int numSlices = 32;

    QGeometryData canRim;
    QVector3D canExtrudeVec(0.0f, 0.0f, -canHeight);

    // do the math for the defining points
    for (int i = 0; i < numSlices; ++i)
    {
        float angle = (float(i) * 2.0 * M_PI) / numSlices;
        canRim.appendVertex(QVector3D(canRadius * cosf(angle),
                                      canRadius * sinf(angle),
                                      canHeight / 2.0f));
    }

    //! [3]
    // create the flat top lid of the can
    builder.newSection();
    builder.currentNode()->setObjectName(QLatin1String("CanTop"));
    QGeometryData top;
    top.appendVertex(canRim.center());
    top.appendVertexArray(canRim.vertices());
    builder.addTriangulatedFace(top);

    // create the sides of the can
    builder.newSection();
    builder.currentNode()->setObjectName(QLatin1String("CanSides"));
    builder.currentNode()->setMaterialIndex(canMat);
    builder.currentNode()->setEffect(QGL::LitModulateTexture2D);
    QGeometryData canTop = canRim;
    canTop.detach();
    canTop.appendVertex(canTop.vertex(0));       // doubled vert for texture seam
    canTop.generateTextureCoordinates();            // generate x texture coords
    QGeometryData canBase = canTop.translated(canExtrudeVec);  // base has tex.y == 0
    for (int i = 0; i < canTop.count(); ++i)
        canTop.texCoord(i).setY(1.0);                      // top has tex.y == 1
    builder.addQuadsInterleaved(canTop, canBase);

    // create the flat bottom lid of the can
    builder.newSection();
    builder.currentNode()->setObjectName(QLatin1String("CanBottom"));
    builder.currentNode()->setEffect(QGL::LitMaterial);
    QGeometryData rimReversed = canRim.translated(canExtrudeVec).reversed();
    QGeometryData canBottom;
    canBottom.appendVertex(rimReversed.center());
    canBottom.appendVertexArray(rimReversed.vertices());
    builder.addTriangulatedFace(canBottom);

    return builder.finalizedSceneNode();
    //! [3]
}
