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
#include <QPropertyAnimation>
#include <QUrl>
#include <Qt3DCore/Window>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DRenderer/QViewport>
#include <Qt3DRenderer/QMaterial>
#include <Qt3DRenderer/QEffect>
#include <Qt3DRenderer/QRenderPass>
#include <Qt3DRenderer/QTechnique>
#include <Qt3DRenderer/QShaderProgram>
#include <Qt3DRenderer/QParameter>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QClearBuffer>
#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QCameraSelector>
#include <Qt3DRenderer/QTechniqueFilter>
#include <Qt3DRenderer/QOpenGLFilter>
#include <qmath.h>

using namespace Qt3D;

int main(int ac, char **av)
{
    QGuiApplication app(ac, av);

    Window view;
    Qt3D::QAspectEngine engine;
    engine.registerAspect(new Qt3D::QRenderAspect());
    engine.initialize();
    QVariantMap data;
    data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(&view)));
    data.insert(QStringLiteral("window"), QVariant::fromValue(&view));
    engine.setData(data);
    QEntity *root = new QEntity();

    // Camera
    QCamera *cameraEntity = new QCamera(root);
    cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, -250.0f, -50.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    view.setCamera(cameraEntity);

    // FrameGraph
    QFrameGraph *frameGraph = new QFrameGraph();
    QTechniqueFilter *techniqueFilter = new QTechniqueFilter();
    QViewport *viewport = new QViewport(techniqueFilter);
    QClearBuffer *clearBuffer = new QClearBuffer(viewport);
    QCameraSelector *cameraSelector = new QCameraSelector(clearBuffer);
    viewport->setRect(QRectF(0, 0, 1, 1));
    clearBuffer->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);
    cameraSelector->setCamera(cameraEntity);
    frameGraph->setActiveFrameGraph(techniqueFilter);
    root->addComponent(frameGraph);

    QShaderProgram *shader = new QShaderProgram();
    shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/diffuse.vert"))));
    shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/diffuse.frag"))));

    QRenderPass *renderPass = new QRenderPass();
    renderPass->setShaderProgram(shader);

    QTechnique *technique = new QTechnique();
    technique->openGLFilter()->setApi(Qt3D::QOpenGLFilter::Desktop);
    technique->openGLFilter()->setMajorVersion(3);
    technique->openGLFilter()->setMinorVersion(1);
    technique->openGLFilter()->setProfile(Qt3D::QOpenGLFilter::Core);
    technique->addPass(renderPass);

    QEffect *effect = new QEffect();
    QParameter* lightPos = new QParameter(QStringLiteral("lightPosition"), QVector4D(10.0f, 10.0f, 0.0f, 1.0f));
    QParameter* lightIntensity = new QParameter(QStringLiteral("lightIntensity"), QVector3D(0.5f, 0.5f, 0.5f));
    effect->addParameter(lightPos);
    effect->addParameter(lightIntensity);
    effect->addTechnique(technique);

    const float radius = 100.0f;
    const int max = 1000;
    const float det = 1.0f / max;
    // Scene
    for (int i = 0; i < max; i++) {
        QEntity *e = new QEntity();
        Qt3D::QTransform *transform = new Qt3D::QTransform();
        QTranslateTransform *translation = new QTranslateTransform();
        QRotateTransform *rotateX = new QRotateTransform();
        QRotateTransform *rotateZ = new QRotateTransform();
        QCylinderMesh *mesh = new QCylinderMesh();
        QMaterial *material = new QMaterial();

        mesh->setRings(50.0f);
        mesh->setSlices(30.0f);
        mesh->setRadius(2.5f);
        mesh->setLength(5.0f);

        const float angle = M_PI * 2.0f * i * det * 10.;

        QParameter *diffuseColorParameter = new QParameter(QStringLiteral("kd"), QVector3D(cosf(angle), 0.3f, sinf(angle)));
        QParameter* ambientColorParameter = new QParameter(QStringLiteral("ka"), QVector3D(cosf(angle), sinf(angle), 0.5f));

        material->setEffect(effect);
        material->addParameter(diffuseColorParameter);
        material->addParameter(ambientColorParameter);

        translation->setTranslation(QVector3D(radius * cos(angle), 200.* i * det, radius * sin(angle)));
        rotateX->setAxis(QVector3D(1.0f, 0.0f, 0.0f));
        rotateZ->setAxis(QVector3D(0.0f, 0.0f, 1.0f));
        rotateX->setAngleDeg(30.0f * i);
        rotateZ->setAngleDeg(45.0f * i);

        QPropertyAnimation *animX = new QPropertyAnimation(rotateX, QByteArrayLiteral("angle"));
        animX->setDuration(2400 * i);
        animX->setStartValue(QVariant::fromValue(i * 30.0f));
        animX->setEndValue(QVariant::fromValue((i + 1) * 390.0f));
        animX->setLoopCount(-1);
        animX->start();

        QPropertyAnimation *animZ = new QPropertyAnimation(rotateZ, QByteArrayLiteral("angle"));
        animZ->setDuration(2400 * i);
        animZ->setStartValue(QVariant::fromValue(i * 20.0f));
        animZ->setEndValue(QVariant::fromValue((i + 1) * 380.0f));
        animZ->setLoopCount(-1);
        animZ->start();

        transform->addTransform(rotateX);
        transform->addTransform(rotateZ);
        transform->addTransform(translation);
        e->addComponent(transform);
        e->addComponent(mesh);
        e->addComponent(material);
        e->setParent(root);
    }

    engine.setRootEntity(root);
    view.show();
    return app.exec();
}
