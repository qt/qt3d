/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "cubeview.h"
#include "qglcube.h"
#include "qglteapot.h"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qpropertyanimation.h>
#include <stdio.h>
#include <qmath.h>
#include "projectivetextureeffect.h"

//#define PROJECTOR_CAMERA_DEBUG_MARKERS


CubeView::CubeView(QWindow *parent)
    : QGLView(parent), scene(0), cube(0), teapot(0), room(0),
      sensitivity(0.1f),
      lightParameters(0),
      showFrameRate(false),
      stereo(false),
      useProjectiveTextureEffect(false),
      cangle(0.0f),
      prevX(0),
      prevY(0),
      prevZ(0),
      havePrev(false),
      projectiveTextureEffect(0)
{
    setOption(CameraNavigation, false);

    roomCamera = new QGLCamera(this);
    roomCamera->setAdjustForAspectRatio(false);

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(this, "cubeAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(5000);
    animation->setLoopCount(-1);
    animation->start();

    time.start();
}

CubeView::~CubeView()
{
    texture.cleanupResources();
}

void CubeView::initializeGL(QGLPainter *painter)
{
    QGLBuilder builder;
    builder << QGL::Faceted << QGLCube(1.0f);
    cube = builder.currentNode();
    builder << QGL::Faceted;
    room = builder.currentNode();
    builder.pushNode();
    QGLSceneNode *back = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, -15.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *left = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, 0.0f));
        quad.appendVertex(QVector3D(-3.0f, -3.0f, 0.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *right = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D(3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D(3.0f, -3.0f, 0.0f));
        quad.appendVertex(QVector3D(3.0f,  3.0f, 0.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *top = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f,  3.0f, 0.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, 0.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *bottom = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f, -3.0f, 0.0f));
        quad.appendVertex(QVector3D( 3.0f, -3.0f, 0.0f));
        quad.appendVertex(QVector3D( 3.0f, -3.0f, -15.0f));
        builder.addQuads(quad);
    }
    builder.popNode();

    int index;
    QSharedPointer<QGLMaterialCollection> palette = builder.sceneNode()->palette();

    QGLMaterial *mat1 = new QGLMaterial();
    mat1->setDiffuseColor(QColor(128, 100, 0));
    index = palette->addMaterial(mat1);
    back->setMaterialIndex(index);

    QGLMaterial *mat2 = new QGLMaterial();
    mat2->setDiffuseColor(Qt::cyan);
    index = palette->addMaterial(mat2);
    left->setMaterialIndex(index);
    right->setMaterialIndex(index);

    QGLMaterial *mat3 = new QGLMaterial();
    mat3->setDiffuseColor(Qt::yellow);
    index = palette->addMaterial(mat3);
    top->setMaterialIndex(index);
    bottom->setMaterialIndex(index);

    //qDumpScene(room);

    builder.newSection();
    builder << QGLTeapot();
    teapot = builder.currentNode();
    QGLMaterial *china = new QGLMaterial();
    china->setAmbientColor(QColor(192, 150, 128));
    china->setSpecularColor(QColor(60, 60, 60));
    china->setShininess(128);
    teapot->setMaterial(china);

    scene = builder.finalizedSceneNode();
    scene->setParent(this);

    roomModel = new QGLLightModel(this);
    roomModel->setAmbientSceneColor(Qt::white);
    roomModel->setViewerPosition(QGLLightModel::LocalViewer);

    normalModel = new QGLLightModel(this);

    lightParameters = new QGLLightParameters(this);
    lightParameters->setPosition(QVector3D(0.0f, 0.0f, 3.0f));
    painter->setMainLight(lightParameters);

    QImage textureImage(QLatin1String(":/qtlogo.png"));
    texture.setImage(textureImage);

    if (stereo) {
        camera()->setEyeSeparation(0.4f);
        roomCamera->setEyeSeparation(0.1f);
    }

    if (useProjectiveTextureEffect)
    {
        // initialize the projector camera
        projectorCamera = new QGLCamera(this);
        projectiveTextureEffect = new ProjectiveTextureEffect;
        connect(projectorCamera, SIGNAL(viewChanged()),
                this, SLOT(updateProjectorViewMatrix()));
        connect(projectorCamera, SIGNAL(projectionChanged()),
                this, SLOT(updateProjectorProjectionMatrix()));
    }
}

void CubeView::paintGL(QGLPainter *painter)
{
    if (showFrameRate)
        qWarning("time since last frame: %d ms", time.restart());

    glDisable(GL_BLEND);

    // Animate the projector position so the effect can be seen
    if (useProjectiveTextureEffect)
    {
        projectorCamera->tiltPanRollCenter
            (-0.1f, -0.3f, 0.0f, QGLCamera::PanTiltRoll);
    }

    painter->modelViewMatrix().push();
    painter->projectionMatrix().push();

    painter->setStandardEffect(QGL::LitMaterial);
    painter->setCamera(roomCamera);
    painter->setLightModel(roomModel);
    room->draw(painter);

    painter->modelViewMatrix().pop();
    painter->projectionMatrix().pop();

    painter->modelViewMatrix().push();
    // These are the model transformations
    painter->modelViewMatrix().translate(-0.8f, -1.5f, -3.0f);
    painter->setLightModel(normalModel);
    if (useProjectiveTextureEffect)
    {
        modelMatrix.push();
        // For an effect that looks like we have only one projector
        // Over the whole screen, we duplicate transformations into the
        // projector's model matrix.  For now, we don't apply the transform
        // to center the effect on each object and see it more clearly.
        // modelMatrix.translate(-0.8f, -1.5f, -3.0f);

        updateProjectiveTextureEffect();

        painter->setUserEffect(projectiveTextureEffect);
        texture.bind();
    }
    else
    {
        painter->setStandardEffect(QGL::LitMaterial);
    }
    teapot->draw(painter);

    if (useProjectiveTextureEffect)
        modelMatrix.pop();
    painter->modelViewMatrix().pop();


    // These are the model transformations
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(1.0f, -0.5f, 0.0f);
    painter->modelViewMatrix().rotate(cangle, 1.0f, 1.0f, 1.0f);

    texture.bind();
    if (useProjectiveTextureEffect)
    {
        modelMatrix.push();
        // For an effect that looks like we have only one projector
        // Over the whole screen, we duplicate transformations into the
        // projector's model matrix.  For now, we don't apply the transform
        // to center the effect on each object and see it more clearly.
//        modelMatrix.translate(1.0f, -0.5f, 0.0f);
        modelMatrix.rotate(cangle, 1.0f, 1.0f, 1.0f);
        updateProjectiveTextureEffect();
        painter->setUserEffect(projectiveTextureEffect);
//        painter->setStandardEffect(QGL::FlatDecalTexture2D);
        cube->draw(painter);
        modelMatrix.pop();
    }
    else
    {
        glEnable(GL_BLEND);
        painter->setStandardEffect(QGL::LitDecalTexture2D);
        painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0, 120));
        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        cube->draw(painter);
        glCullFace(GL_BACK);
        cube->draw(painter);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    painter->modelViewMatrix().pop();
#ifdef PROJECTOR_CAMERA_DEBUG_MARKERS
    if (useProjectiveTextureEffect)
    {

        painter->modelViewMatrix().push();
        modelMatrix.push();
        painter->modelViewMatrix().translate(projectorCamera->eye());
        painter->modelViewMatrix().scale(0.2);
        painter->setStandardEffect(QGL::LitMaterial);
        modelMatrix.translate(projectorCamera->eye());
        cube->draw(painter);

        modelMatrix.pop();
        painter->modelViewMatrix().pop();

        painter->modelViewMatrix().push();
        modelMatrix.push();
        painter->modelViewMatrix().translate(projectorCamera->center());
        painter->modelViewMatrix().scale(0.1);
        painter->setStandardEffect(QGL::LitMaterial);
        cube->draw(painter);
        modelMatrix.pop();
        painter->modelViewMatrix().pop();

        QVector3DArray verts;

        QVector3D origin = projectorCamera->eye();

        QVector3D target = projectorCamera->center();
        QVector3D direction = projectorCamera->center() - projectorCamera->eye();

        QVector3D normal = projectorCamera->upVector().normalized();
        float nearPlane = projectorCamera->nearPlane();
        float farPlane = projectorCamera->farPlane();
        float fieldOfView = projectorCamera->fieldOfView();

        QVector3D nearTopLeft;
        QVector3D nearTopRight;
        QVector3D nearBottomLeft;
        QVector3D nearBottomRight;
        QVector3D farTopLeft;
        QVector3D farTopRight;
        QVector3D farBottomLeft;
        QVector3D farBottomRight;

        QSizeF viewSize = projectorCamera->viewSize();

        float fieldDepthRatio = farPlane / nearPlane;

        QVector3D rightVector = QVector3D::crossProduct(direction, normal).normalized() * viewSize.width() / 2.0;
        QVector3D topVector = normal * viewSize.height() / 2.0;

        QVector3D topLeftVector = direction + topVector - rightVector;
        QVector3D topRightVector = direction + topVector + rightVector;
        QVector3D bottomLeftVector = direction - topVector - rightVector;
        QVector3D bottomRightVector = direction - topVector + rightVector;

        verts.append(origin, origin + (direction * (farPlane / direction.length())));

        verts.append(origin, origin + (topLeftVector * fieldDepthRatio));
        verts.append(origin, origin + (topRightVector * fieldDepthRatio));
        verts.append(origin, origin + (bottomLeftVector * fieldDepthRatio));
        verts.append(origin, origin + (bottomRightVector * fieldDepthRatio));

        verts.append(origin + topLeftVector, origin + topRightVector);
        verts.append(origin + topRightVector, origin + bottomRightVector);
        verts.append(origin + bottomRightVector, origin + bottomLeftVector);
        verts.append(origin + bottomLeftVector, origin + topLeftVector);

        verts.append(origin + (topLeftVector * fieldDepthRatio),
                     (origin + topRightVector * fieldDepthRatio));
        verts.append(origin + (topRightVector * fieldDepthRatio),
                     (origin + bottomRightVector * fieldDepthRatio));
        verts.append(origin + (bottomRightVector * fieldDepthRatio),
                     (origin + bottomLeftVector * fieldDepthRatio));
        verts.append(origin + (bottomLeftVector * fieldDepthRatio),
                     (origin + topLeftVector * fieldDepthRatio));

        verts.append(origin, origin + normal);

        painter->modelViewMatrix().push();
        painter->setStandardEffect(QGL::FlatColor);
        painter->clearAttributes();
        painter->setVertexAttribute(QGL::Position, QGLAttributeValue(verts));
        glLineWidth(1.0f);

        painter->setColor(QColor(255,255,255,255));
        painter->draw(QGL::Lines, verts.size());
        painter->modelViewMatrix().pop();
    }
#endif
}

//inline void CubeView::setProjectiveTextureEffect(bool value)
//{
//    useProjectiveTextureEffect = value;
//}

void CubeView::setCubeAngle(float angle)
{
    cangle = angle;
    accelerometerTimeout();
    update();
}

void CubeView::accelerometerTimeout()
{
    QVector3D g = gravity();
    camera()->setMotionAdjustment(g);
    roomCamera->setMotionAdjustment(g);
}

QVector3D CubeView::gravity() const
{
    // Access the raw accelerometer data on the N900.
    FILE *file = fopen("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "r");
    if (!file)
        return QVector3D(0.0f, 0.0f, -1.0f);
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    int numValues = fscanf(file, "%f %f %f", &x, &y, &z);
    fclose(file);

    if (numValues!=3)
        qWarning("i2c adaptor returned less then 3 values on fscanf.");

    // Smooth out the reported values.  Large changes are applied as-is,
    // and small jitters smooth to the rest position.
    if (havePrev) {
        float xdiff = x - prevX;
        float ydiff = y - prevY;
        float zdiff = z - prevZ;
        if (qAbs(xdiff) < 20.0f && qAbs(ydiff) < 20.0f && qAbs(zdiff) < 20.0f) {
            x = prevX + xdiff * 0.1f;
            y = prevY + ydiff * 0.1f;
            z = prevZ + zdiff * 0.1f;
        }
    }
    prevX = x;
    prevY = y;
    prevZ = z;
    havePrev = true;

    return QVector3D((x / 1000.0f) * sensitivity,
                     (-y / 1000.0f) * sensitivity, -z / 1000.0f);
}

void CubeView::updateProjectorViewMatrix()
{
    Q_ASSERT_X(projectorCamera != 0, Q_FUNC_INFO, "Null projector camera in updateProjectorViewMatrix()");
    projectiveTextureEffect->setProjectorViewMatrix(projectorCamera->modelViewMatrix());
    updateProjectiveTextureEffect();
}

void CubeView::updateProjectorProjectionMatrix()
{
    float projectorAspectRatio = 1.0f;
    projectiveTextureEffect->setProjectorProjectionMatrix(projectorCamera->projectionMatrix(projectorAspectRatio));
    updateProjectiveTextureEffect();
}

void CubeView::updateProjectiveTextureEffect()
{
    projectiveTextureEffect->setProjectorDirection(projectorCamera->center() - projectorCamera->eye());
    projectiveTextureEffect->setModelMatrix(modelMatrix);
}
