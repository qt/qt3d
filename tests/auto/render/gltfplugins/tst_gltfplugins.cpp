/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/qtest.h>
#include <QtCore/qtemporarydir.h>
#include <QtGui/qimage.h>

#include <private/qsceneimporter_p.h>
#include <private/qsceneexportfactory_p.h>
#include <private/qsceneexporter_p.h>
#include <private/qsceneimportfactory_p.h>
#include <private/qsceneimporter_p.h>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qtextureimage.h>
#include <Qt3DRender/qspotlight.h>
#include <Qt3DRender/qdirectionallight.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qeffect.h>

#include <Qt3DExtras/qconemesh.h>
#include <Qt3DExtras/qcuboidmesh.h>
#include <Qt3DExtras/qcylindermesh.h>
#include <Qt3DExtras/qplanemesh.h>
#include <Qt3DExtras/qspheremesh.h>
#include <Qt3DExtras/qtorusmesh.h>
#include <Qt3DExtras/qt3dwindow.h>
#include <Qt3DExtras/qphongmaterial.h>
#include <Qt3DExtras/qphongalphamaterial.h>
#include <Qt3DExtras/qdiffusemapmaterial.h>
#include <Qt3DExtras/qdiffusespecularmapmaterial.h>
#include <Qt3DExtras/qnormaldiffusemapmaterial.h>
#include <Qt3DExtras/qnormaldiffusemapalphamaterial.h>
#include <Qt3DExtras/qnormaldiffusespecularmapmaterial.h>
#include <Qt3DExtras/qgoochmaterial.h>
#include <Qt3DExtras/qpervertexcolormaterial.h>
#include <Qt3DExtras/qforwardrenderer.h>

//#define VISUAL_CHECK 5000  // The value indicates the time for visual check in ms
//#define PRESERVE_EXPORT  // Uncomment to preserve export directory contents for analysis

class tst_gltfPlugins : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void init();
    void cleanup();
    void exportAndImport_data();
    void exportAndImport();

private:
    void createTestScene();
    Qt3DCore::QEntity *findCameraChild(Qt3DCore::QEntity *entity,
                                       Qt3DRender::QCameraLens::ProjectionType type);
    void walkEntity(Qt3DCore::QEntity *entity, int depth);
    void createAndAddEntity(const QString &name,
                            Qt3DCore::QComponent *comp1 = nullptr,
                            Qt3DCore::QComponent *comp2 = nullptr,
                            Qt3DCore::QComponent *comp3 = nullptr,
                            Qt3DCore::QEntity *parent = nullptr);
    void addPositionAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                        Qt3DRender::QBuffer *buffer, int count);
    void addIndexAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                     Qt3DRender::QBuffer *buffer, int count);
    void addColorAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                     Qt3DRender::QBuffer *buffer, int count);
    Qt3DCore::QEntity *findChildEntity(Qt3DCore::QEntity *entity, const QString &name);
    Qt3DCore::QTransform *transformComponent(Qt3DCore::QEntity *entity);
    Qt3DRender::QAbstractLight *lightComponent(Qt3DCore::QEntity *entity);
    Qt3DRender::QCameraLens *cameraComponent(Qt3DCore::QEntity *entity);
    Qt3DRender::QGeometryRenderer *meshComponent(Qt3DCore::QEntity *entity);
    Qt3DRender::QMaterial *materialComponent(Qt3DCore::QEntity *entity);
    void compareComponents(Qt3DCore::QComponent *c1, Qt3DCore::QComponent *c2);
    Qt3DRender::QAttribute *findAttribute(const QString &name,
                                          Qt3DRender::QAttribute::AttributeType type,
                                          Qt3DRender::QGeometry *geometry);
    void compareAttributes(Qt3DRender::QAttribute *a1, Qt3DRender::QAttribute *a2);

    QTemporaryDir *m_exportDir;
    Qt3DExtras::Qt3DWindow *m_view1;
    Qt3DExtras::Qt3DWindow *m_view2;
    Qt3DCore::QEntity *m_sceneRoot1;
    Qt3DCore::QEntity *m_sceneRoot2;
    QHash<QString, Qt3DCore::QEntity *> m_entityMap;
};

void tst_gltfPlugins::initTestCase()
{
#ifndef VISUAL_CHECK
    // QEffect doesn't get registered unless aspects are initialized, generating warnings in
    // material comparisons.
    qRegisterMetaType<Qt3DRender::QEffect *>();
#endif
}

void tst_gltfPlugins::init()
{
    m_exportDir = new QTemporaryDir;
#ifdef VISUAL_CHECK
    m_view1 = new Qt3DExtras::Qt3DWindow;
    m_view1->setTitle(QStringLiteral("Original scene"));
    m_view2 = new Qt3DExtras::Qt3DWindow;
    m_view2->setTitle(QStringLiteral("Imported scene"));
#endif
}

void tst_gltfPlugins::cleanup()
{
    delete m_sceneRoot1;
    delete m_sceneRoot2;
    m_entityMap.clear();
#ifdef VISUAL_CHECK
    delete m_view1;
    delete m_view2;
#endif
    delete m_exportDir;
    // Make sure the slate is clean for the next case
    QCoreApplication::processEvents();
    QTest::qWait(0);
}

void tst_gltfPlugins::walkEntity(Qt3DCore::QEntity *entity, int depth)
{
    QString indent;
    indent.fill(' ', depth * 2);
    qDebug().noquote() << indent << "Entity:" << entity << "Components:" << entity->components();
    for (auto child : entity->children()) {
        if (auto childEntity = qobject_cast<Qt3DCore::QEntity *>(child))
            walkEntity(childEntity, depth + 1);
    }
}

void tst_gltfPlugins::createAndAddEntity(const QString &name,
                                         Qt3DCore::QComponent *comp1,
                                         Qt3DCore::QComponent *comp2,
                                         Qt3DCore::QComponent *comp3,
                                         Qt3DCore::QEntity *parent)
{
    Qt3DCore::QEntity *parentEntity = parent ? parent : m_sceneRoot1;
    Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(parentEntity);
    entity->setObjectName(name);
    if (comp1) {
        entity->addComponent(comp1);
        comp1->setObjectName(comp1->metaObject()->className());
    }
    if (comp2) {
        entity->addComponent(comp2);
        comp2->setObjectName(comp2->metaObject()->className());
    }
    if (comp3) {
        entity->addComponent(comp3);
        comp3->setObjectName(comp3->metaObject()->className());
    }

    m_entityMap.insert(name, entity);
}

void tst_gltfPlugins::createTestScene()
{
#ifdef VISUAL_CHECK
    m_view1->defaultFrameGraph()->setClearColor(Qt::lightGray);
    m_view2->defaultFrameGraph()->setClearColor(Qt::lightGray);
#endif
    m_sceneRoot1 = new Qt3DCore::QEntity();
    m_sceneRoot1->setObjectName(QStringLiteral("Scene root"));
    m_sceneRoot2 = new Qt3DCore::QEntity();
    m_sceneRoot2->setObjectName(QStringLiteral("Imported scene parent"));

    // Perspective camera
    {
        Qt3DRender::QCamera *camera = new Qt3DRender::QCamera(m_sceneRoot1);
        camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
        camera->setViewCenter(QVector3D(0.0f, 1.5f, 0.0f));
        camera->setPosition(QVector3D(0.0f, 3.5f, 15.0f));
        camera->setNearPlane(0.001f);
        camera->setFarPlane(10000.0f);
        camera->setObjectName(QStringLiteral("Main camera"));
        camera->transform()->setObjectName(QStringLiteral("Main camera transform"));
        camera->lens()->setObjectName(QStringLiteral("Main camera lens"));
        camera->setFieldOfView(30.0f);
        camera->setAspectRatio(1.0f);
    }
    // Ortho camera
    {
        Qt3DCore::QEntity *camera = new Qt3DCore::QEntity(m_sceneRoot1);
        camera->setObjectName(QStringLiteral("Ortho camera"));

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(0.0f, 0.0f, -15.0f));
        transform->setObjectName(QStringLiteral("Ortho camera transform"));
        camera->addComponent(transform);

        Qt3DRender::QCameraLens *lens = new Qt3DRender::QCameraLens;
        lens->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
        lens->setNearPlane(0.001f);
        lens->setFarPlane(10000.0f);
        lens->setRight(7.0f);
        lens->setLeft(-7.0f);
        lens->setTop(5.0f);
        lens->setBottom(-5.0f);
        lens->setObjectName(QStringLiteral("Ortho camera lens"));
        camera->addComponent(lens);

#ifdef VISUAL_CHECK
        m_view1->defaultFrameGraph()->setCamera(camera);
#endif
    }

    // Point light
    {
        Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight;
        light->setColor(QColor("#FFDDAA"));
        light->setIntensity(0.9f);
        light->setConstantAttenuation(0.03f);
        light->setLinearAttenuation(0.04f);
        light->setQuadraticAttenuation(0.01f);
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(-6.0f, 2.0f, 3.0f));
        createAndAddEntity(QStringLiteral("Point Light"), light, transform);
    }
    // Directional light
    {
        Qt3DRender::QDirectionalLight *light = new Qt3DRender::QDirectionalLight;
        light->setColor(QColor("#BBCCEE"));
        light->setIntensity(0.75f);
        light->setWorldDirection(QVector3D(-1.0f, -1.0f, -1.0f));
        createAndAddEntity(QStringLiteral("Directional Light"), light);
    }
    // Spot light
    {
        Qt3DRender::QSpotLight *light = new Qt3DRender::QSpotLight;
        light->setColor(QColor("#5599DD"));
        light->setIntensity(2.0f);
        light->setConstantAttenuation(0.03f);
        light->setLinearAttenuation(0.04f);
        light->setQuadraticAttenuation(0.01f);
        light->setLocalDirection(QVector3D(0.0f, -1.0f, -1.0f));
        light->setCutOffAngle(30.0f);
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(0.0f, 5.0f, 0.0f));
        createAndAddEntity(QStringLiteral("Spot Light"), light, transform);
    }
    // Cube with DiffuseMap
    {
        Qt3DExtras::QDiffuseMapMaterial *material = new Qt3DExtras::QDiffuseMapMaterial();
        Qt3DRender::QTextureImage *diffuseTextureImage = new Qt3DRender::QTextureImage();
        material->diffuse()->addTextureImage(diffuseTextureImage);
        material->setAmbient(QColor("#000088"));
        material->setSpecular(QColor("#FFFF00"));
        material->setShininess(30.0);
        material->setTextureScale(2.0f);
        diffuseTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo.png")));
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setScale(0.75f);
        transform->setTranslation(QVector3D(2.0f, 1.0f, -1.0f));
        Qt3DExtras::QCuboidMesh *mesh = new Qt3DExtras::QCuboidMesh;
        mesh->setXExtent(1.2f);
        mesh->setYExtent(1.1f);
        mesh->setZExtent(0.9f);
        mesh->setYZMeshResolution(QSize(2, 2));
        mesh->setYZMeshResolution(QSize(2, 3));
        mesh->setYZMeshResolution(QSize(3, 2));
        createAndAddEntity(QStringLiteral("Cube with DiffuseMap"), mesh, material, transform);
    }
    // Cone with PhongAlpha
    {
        Qt3DExtras::QPhongAlphaMaterial *material = new Qt3DExtras::QPhongAlphaMaterial();
        material->setAlpha(0.6f);
        material->setAmbient(QColor("#550000"));
        material->setDiffuse(QColor("#00FFFF"));
        material->setSpecular(QColor("#FFFF00"));
        material->setShininess(20.0f);
        material->setSourceRgbArg(Qt3DRender::QBlendEquationArguments::Source1Color);
        material->setSourceAlphaArg(Qt3DRender::QBlendEquationArguments::Source1Alpha);
        material->setDestinationRgbArg(Qt3DRender::QBlendEquationArguments::DestinationColor);
        material->setDestinationAlphaArg(Qt3DRender::QBlendEquationArguments::DestinationAlpha);
        material->setBlendFunctionArg(Qt3DRender::QBlendEquation::ReverseSubtract);
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(2.0f, 1.0f, 1.0f));
        transform->setRotation(Qt3DCore::QTransform::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -20.0f));
        Qt3DExtras::QConeMesh *mesh = new Qt3DExtras::QConeMesh;
        mesh->setRings(2);
        mesh->setSlices(16);
        mesh->setTopRadius(0.5f);
        mesh->setBottomRadius(1.5f);
        mesh->setLength(0.9f);
        createAndAddEntity(QStringLiteral("Cone with PhongAlpha"), mesh, material, transform);
    }
    // Cylinder with Phong
    {
        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setAmbient(QColor("#220022"));
        material->setDiffuse(QColor("#6633AA"));
        material->setSpecular(QColor("#66AA33"));
        material->setShininess(50.0f);
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(0.0f, 1.0f, 1.0f));
        transform->setRotation(Qt3DCore::QTransform::fromAxisAndAngle(0.0f, 0.0f, 1.0f, -45.0f));
        Qt3DExtras::QCylinderMesh *mesh = new Qt3DExtras::QCylinderMesh;
        mesh->setRadius(0.5f);
        mesh->setRings(3);
        mesh->setLength(1.2f);
        mesh->setSlices(16);
        createAndAddEntity(QStringLiteral("Cylinder with Phong"), mesh, material, transform);
    }
    // Plane with DiffuseSpecularMap
    {
        Qt3DExtras::QDiffuseSpecularMapMaterial *material =
                new Qt3DExtras::QDiffuseSpecularMapMaterial();
        Qt3DRender::QTextureImage *diffuseTextureImage = new Qt3DRender::QTextureImage();
        material->diffuse()->addTextureImage(diffuseTextureImage);
        diffuseTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo.png")));
        Qt3DRender::QTextureImage *specularTextureImage = new Qt3DRender::QTextureImage();
        material->specular()->addTextureImage(specularTextureImage);
        specularTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo_specular.png")));

        material->setAmbient(QColor("#0000FF"));
        material->setTextureScale(3.0f);
        material->setShininess(15.0f);

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(-1.0f, 1.0f, 1.0f));
        transform->setRotation(Qt3DCore::QTransform::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 45.0f));

        Qt3DExtras::QPlaneMesh *mesh = new Qt3DExtras::QPlaneMesh;
        mesh->setMeshResolution(QSize(3, 3));
        mesh->setHeight(1.5f);
        mesh->setWidth(1.2f);
        createAndAddEntity(QStringLiteral("Plane with DiffuseSpecularMap"),
                           mesh, material, transform);
    }
    // Sphere with NormalDiffuseMap
    {
        Qt3DExtras::QNormalDiffuseMapMaterial *material =
                new Qt3DExtras::QNormalDiffuseMapMaterial();

        Qt3DRender::QTextureImage *normalTextureImage = new Qt3DRender::QTextureImage();
        material->normal()->addTextureImage(normalTextureImage);
        normalTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo_normal.png")));

        Qt3DRender::QTextureImage *diffuseTextureImage = new Qt3DRender::QTextureImage();
        material->diffuse()->addTextureImage(diffuseTextureImage);
        diffuseTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo.png")));

        material->setAmbient(QColor("#000044"));
        material->setSpecular(QColor("#0000CC"));
        material->setShininess(9.0f);
        material->setTextureScale(4.0f);

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(0.0f, 1.0f, -10.0f));

        Qt3DExtras::QSphereMesh *mesh = new Qt3DExtras::QSphereMesh;
        mesh->setRadius(2.0f);
        mesh->setRings(16);
        mesh->setSlices(16);
        mesh->setGenerateTangents(true);
        createAndAddEntity(QStringLiteral("Sphere with NormalDiffuseMap"),
                           mesh, material, transform);
    }
    // Sphere with NormalDiffuseMapAlpha
    {
        Qt3DExtras::QNormalDiffuseMapAlphaMaterial *material =
                new Qt3DExtras::QNormalDiffuseMapAlphaMaterial();

        Qt3DRender::QTextureImage *normalTextureImage = new Qt3DRender::QTextureImage();
        material->normal()->addTextureImage(normalTextureImage);
        normalTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo_normal.png")));

        Qt3DRender::QTextureImage *diffuseTextureImage = new Qt3DRender::QTextureImage();
        material->diffuse()->addTextureImage(diffuseTextureImage);
        diffuseTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo_with_alpha.png")));

        material->setAmbient(QColor("#000044"));
        material->setSpecular(QColor("#0000CC"));
        material->setShininess(9.0f);
        material->setTextureScale(4.0f);

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(4.0f, 1.0f, -10.0f));

        Qt3DExtras::QSphereMesh *mesh = new Qt3DExtras::QSphereMesh;
        mesh->setRadius(2.0f);
        mesh->setRings(16);
        mesh->setSlices(16);
        mesh->setGenerateTangents(true);
        createAndAddEntity(QStringLiteral("Sphere with NormalDiffuseMapAlpha"),
                           mesh, material, transform);
    }
    // Sphere with NormalDiffuseSpecularMap
    {
        Qt3DExtras::QNormalDiffuseSpecularMapMaterial *material =
                new Qt3DExtras::QNormalDiffuseSpecularMapMaterial();

        Qt3DRender::QTextureImage *normalTextureImage = new Qt3DRender::QTextureImage();
        material->normal()->addTextureImage(normalTextureImage);
        normalTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo_normal.png")));

        Qt3DRender::QTextureImage *diffuseTextureImage = new Qt3DRender::QTextureImage();
        material->diffuse()->addTextureImage(diffuseTextureImage);
        diffuseTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo.png")));

        Qt3DRender::QTextureImage *specularTextureImage = new Qt3DRender::QTextureImage();
        material->specular()->addTextureImage(specularTextureImage);
        specularTextureImage->setSource(QUrl(QStringLiteral("qrc:/qtlogo_specular.png")));

        material->setAmbient(QColor("#000044"));
        material->setShininess(9.0f);
        material->setTextureScale(4.0f);

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(-4.0f, 1.0f, -10.0f));

        Qt3DExtras::QSphereMesh *mesh = new Qt3DExtras::QSphereMesh;
        mesh->setRadius(2.0f);
        mesh->setRings(16);
        mesh->setSlices(16);
        mesh->setGenerateTangents(true);
        createAndAddEntity(QStringLiteral("Sphere with NormalDiffuseSpecularMap"),
                           mesh, material, transform);
    }
    // Torus with Gooch
    {
        Qt3DExtras::QGoochMaterial *material = new Qt3DExtras::QGoochMaterial();

        material->setDiffuse(QColor("#333333"));
        material->setSpecular(QColor("#550055"));
        material->setCool(QColor("#0055AA"));
        material->setWarm(QColor("#FF3300"));
        material->setAlpha(0.2f);
        material->setBeta(0.4f);
        material->setShininess(22.0f);

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(0.0f, 4.0f, -10.0f));

        Qt3DExtras::QTorusMesh *mesh = new Qt3DExtras::QTorusMesh;
        mesh->setRadius(1.0f);
        mesh->setMinorRadius(0.5f);
        mesh->setRings(16);
        mesh->setSlices(16);
        createAndAddEntity(QStringLiteral("Torus with Gooch"), mesh, material, transform);
    }
    // Custom cube with per-vertex colors
    {
        Qt3DExtras::QPerVertexColorMaterial *material = new Qt3DExtras::QPerVertexColorMaterial();
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(4.0f, 3.0f, -15.0f));
        transform->setRotation(Qt3DCore::QTransform::fromAxisAndAngle(1.0f, 1.0f, 1.0f, 270.0f));

        Qt3DRender::QGeometryRenderer *boxMesh = new Qt3DRender::QGeometryRenderer();
        Qt3DRender::QGeometry *boxGeometry = new Qt3DRender::QGeometry(boxMesh);
        Qt3DRender::QBuffer *boxDataBuffer =
                new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, boxGeometry);
        Qt3DRender::QBuffer *colorDataBuffer =
                new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, boxGeometry);
        Qt3DRender::QBuffer *indexDataBuffer =
                new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, boxGeometry);
        QByteArray vertexBufferData;
        QByteArray colorBufferData;
        QByteArray indexBufferData;

        vertexBufferData.resize(8 * 3 * sizeof(float));
        colorBufferData.resize(8 * 4 * sizeof(float));
        indexBufferData.resize(12 * 3 * sizeof(ushort));

        float dimension = 1.0f;

        float *vPtr = reinterpret_cast<float *>(vertexBufferData.data());
        vPtr[0]  = -dimension; vPtr[1]  = -dimension; vPtr[2]  = -dimension;
        vPtr[3]  = dimension;  vPtr[4]  = -dimension; vPtr[5]  = -dimension;
        vPtr[6]  = dimension;  vPtr[7]  = -dimension; vPtr[8]  = dimension;
        vPtr[9]  = -dimension; vPtr[10] = -dimension; vPtr[11] = dimension;
        vPtr[12] = -dimension; vPtr[13] = dimension;  vPtr[14] = -dimension;
        vPtr[15] = dimension;  vPtr[16] = dimension;  vPtr[17] = -dimension;
        vPtr[18] = dimension;  vPtr[19] = dimension;  vPtr[20] = dimension;
        vPtr[21] = -dimension; vPtr[22] = dimension;  vPtr[23] = dimension;

        float *cPtr = reinterpret_cast<float *>(colorBufferData.data());
        for (int i = 0; i < 8; i++) {
            cPtr[i * 4] = vPtr[i * 3];
            cPtr[i * 4 + 1] = vPtr[i * 3 + 1];
            cPtr[i * 4 + 2] = vPtr[i * 3 + 2];
            cPtr[i * 4 + 3] = 1.0f;
        }

        ushort *iPtr = reinterpret_cast<ushort *>(indexBufferData.data());
        iPtr[0]  = 2; iPtr[1]  = 0; iPtr[2]  = 1;
        iPtr[3]  = 2; iPtr[4]  = 3; iPtr[5]  = 0;
        iPtr[6]  = 1; iPtr[7]  = 6; iPtr[8]  = 2;
        iPtr[9]  = 1; iPtr[10] = 5; iPtr[11] = 6;
        iPtr[12] = 2; iPtr[13] = 7; iPtr[14] = 3;
        iPtr[15] = 2; iPtr[16] = 6; iPtr[17] = 7;
        iPtr[18] = 6; iPtr[19] = 5; iPtr[20] = 4;
        iPtr[21] = 6; iPtr[22] = 4; iPtr[23] = 7;
        iPtr[24] = 7; iPtr[25] = 0; iPtr[26] = 3;
        iPtr[27] = 7; iPtr[28] = 4; iPtr[29] = 0;
        iPtr[30] = 4; iPtr[31] = 1; iPtr[32] = 0;
        iPtr[33] = 4; iPtr[34] = 5; iPtr[35] = 1;

        boxDataBuffer->setData(vertexBufferData);
        colorDataBuffer->setData(colorBufferData);
        indexDataBuffer->setData(indexBufferData);

        addPositionAttributeToGeometry(boxGeometry, boxDataBuffer, 8);
        addColorAttributeToGeometry(boxGeometry, colorDataBuffer, 8);
        addIndexAttributeToGeometry(boxGeometry, indexDataBuffer, 36);

        boxMesh->setInstanceCount(1);
        boxMesh->setIndexOffset(0);
        boxMesh->setFirstInstance(0);
        boxMesh->setVertexCount(36);
        boxMesh->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
        boxMesh->setGeometry(boxGeometry);

        createAndAddEntity(QStringLiteral("Custom cube with per-vertex colors"),
                           boxMesh, material, transform);
    }
    // Child cylinder with Phong
    {
        Qt3DCore::QEntity *parentEntity = findChildEntity(m_sceneRoot1,
                                                          QStringLiteral("Cylinder with Phong"));
        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setAmbient(QColor("#333333"));
        material->setDiffuse(QColor("#88FF00"));
        material->setSpecular(QColor("#000088"));
        material->setShininess(150.0f);
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setTranslation(QVector3D(0.0f, 4.0f, 0.0f));
        Qt3DExtras::QCylinderMesh *mesh = new Qt3DExtras::QCylinderMesh;
        mesh->setRadius(0.25f);
        mesh->setRings(3);
        mesh->setLength(1.5f);
        mesh->setSlices(16);
        createAndAddEntity(QStringLiteral("Child with Phong"),
                           mesh, material, transform, parentEntity);
    }
#ifdef VISUAL_CHECK
    m_view1->setGeometry(30, 30, 400, 400);
    m_view1->setRootEntity(m_sceneRoot1);
    m_view1->show();

    m_view2->setGeometry(450, 30, 400, 400);
    m_view2->setRootEntity(m_sceneRoot2);
    m_view2->show();

    QTest::qWaitForWindowExposed(m_view1);
    QTest::qWaitForWindowExposed(m_view2);
#endif
}

void tst_gltfPlugins::addPositionAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                                     Qt3DRender::QBuffer *buffer, int count)
{
    Qt3DRender::QAttribute *posAttribute = new Qt3DRender::QAttribute();
    posAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    posAttribute->setBuffer(buffer);
    posAttribute->setDataType(Qt3DRender::QAttribute::Float);
    posAttribute->setDataSize(3);
    posAttribute->setByteOffset(0);
    posAttribute->setByteStride(0);
    posAttribute->setCount(count);
    posAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    geometry->addAttribute(posAttribute);
}

void tst_gltfPlugins::addIndexAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                                  Qt3DRender::QBuffer *buffer, int count)
{
    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(buffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(count);

    geometry->addAttribute(indexAttribute);
}

void tst_gltfPlugins::addColorAttributeToGeometry(Qt3DRender::QGeometry *geometry,
                                                  Qt3DRender::QBuffer *buffer, int count)
{
    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(buffer);
    colorAttribute->setDataType(Qt3DRender::QAttribute::Float);
    colorAttribute->setDataSize(4);
    colorAttribute->setByteOffset(0);
    colorAttribute->setByteStride(0);
    colorAttribute->setCount(count);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    geometry->addAttribute(colorAttribute);
}

Qt3DCore::QEntity *tst_gltfPlugins::findChildEntity(Qt3DCore::QEntity *entity, const QString &name)
{
    for (auto child : entity->children()) {
        if (auto childEntity = qobject_cast<Qt3DCore::QEntity *>(child)) {
            if (childEntity->objectName() == name)
                return childEntity;
            if (auto foundEntity = findChildEntity(childEntity, name))
                return foundEntity;
        }
    }
    return nullptr;
}

Qt3DCore::QTransform *tst_gltfPlugins::transformComponent(Qt3DCore::QEntity *entity)
{
    for (auto component : entity->components()) {
        if (auto castedComponent = qobject_cast<Qt3DCore::QTransform *>(component))
            return castedComponent;
    }
    return nullptr;
}

Qt3DRender::QAbstractLight *tst_gltfPlugins::lightComponent(Qt3DCore::QEntity *entity)
{
    for (auto component : entity->components()) {
        if (auto castedComponent = qobject_cast<Qt3DRender::QAbstractLight *>(component))
            return castedComponent;
    }
    return nullptr;
}

Qt3DRender::QCameraLens *tst_gltfPlugins::cameraComponent(Qt3DCore::QEntity *entity)
{
    for (auto component : entity->components()) {
        if (auto castedComponent = qobject_cast<Qt3DRender::QCameraLens *>(component))
            return castedComponent;
    }
    return nullptr;
}

Qt3DRender::QGeometryRenderer *tst_gltfPlugins::meshComponent(Qt3DCore::QEntity *entity)
{
    for (auto component : entity->components()) {
        if (auto castedComponent = qobject_cast<Qt3DRender::QGeometryRenderer *>(component))
            return castedComponent;
    }
    return nullptr;
}

Qt3DRender::QMaterial *tst_gltfPlugins::materialComponent(Qt3DCore::QEntity *entity)
{
    for (auto component : entity->components()) {
        if (auto castedComponent = qobject_cast<Qt3DRender::QMaterial *>(component))
            return castedComponent;
    }
    return nullptr;
}

void tst_gltfPlugins::compareComponents(Qt3DCore::QComponent *c1, Qt3DCore::QComponent *c2)
{
    // Make sure component classes are the same and the non-pointer properties are the same
    QCOMPARE((c1 == nullptr), (c2 == nullptr));
    if (c1) {
        // Transform names are lost in export, as the transform is just part of the node item
        if (!qobject_cast<Qt3DCore::QTransform *>(c1))
            QCOMPARE(c1->objectName(), c2->objectName());
        // Meshes are all imported as generic meshes
        if (auto mesh1 = qobject_cast<Qt3DRender::QGeometryRenderer *>(c1)) {
            auto mesh2 = qobject_cast<Qt3DRender::QGeometryRenderer *>(c2);
            QVERIFY(mesh2 != nullptr);
            auto geometry1 = mesh1->geometry();
            auto geometry2 = mesh2->geometry();
            // Check that attributes match.
            compareAttributes(
                        findAttribute(Qt3DRender::QAttribute::defaultPositionAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry1),
                        findAttribute(Qt3DRender::QAttribute::defaultPositionAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry2));
            compareAttributes(
                        findAttribute(Qt3DRender::QAttribute::defaultNormalAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry1),
                        findAttribute(Qt3DRender::QAttribute::defaultNormalAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry2));
            compareAttributes(
                        findAttribute(Qt3DRender::QAttribute::defaultTangentAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry1),
                        findAttribute(Qt3DRender::QAttribute::defaultTangentAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry2));
            compareAttributes(
                        findAttribute(Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry1),
                        findAttribute(Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry2));
            compareAttributes(
                        findAttribute(Qt3DRender::QAttribute::defaultColorAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry1),
                        findAttribute(Qt3DRender::QAttribute::defaultColorAttributeName(),
                                      Qt3DRender::QAttribute::VertexAttribute,
                                      geometry2));
            compareAttributes(
                        findAttribute(QStringLiteral(""),
                                      Qt3DRender::QAttribute::IndexAttribute,
                                      geometry1),
                        findAttribute(QStringLiteral(""),
                                      Qt3DRender::QAttribute::IndexAttribute,
                                      geometry2));
        } else {
            QCOMPARE(c1->metaObject()->className(), c2->metaObject()->className());
            int count = c1->metaObject()->propertyCount();
            for (int i = 0; i < count; i++) {
                auto property = c1->metaObject()->property(i);
                auto v1 = c1->property(property.name());
                auto v2 = c2->property(property.name());
                if (v1.type() == QMetaType::Bool) {
                    QCOMPARE(v1.toBool(), v2.toBool());
                } else if (v1.type() == QVariant::Color) {
                    QCOMPARE(v1.value<QColor>(), v2.value<QColor>());
                } else if (v1.type() == QVariant::Vector3D) {
                    QCOMPARE(v1.value<QVector3D>(), v2.value<QVector3D>());
                } else if (v1.type() == QVariant::Matrix4x4) {
                    QCOMPARE(v1.value<QMatrix4x4>(), v2.value<QMatrix4x4>());
                } else if (v1.canConvert(QMetaType::Float)) {
                    QVERIFY(qFuzzyCompare(v1.toFloat(), v2.toFloat()));
                }
            }
        }
    }
}

Qt3DRender::QAttribute *tst_gltfPlugins::findAttribute(const QString &name,
                                                       Qt3DRender::QAttribute::AttributeType type,
                                                       Qt3DRender::QGeometry *geometry)
{
    for (auto att : geometry->attributes()) {
        if ((type == Qt3DRender::QAttribute::IndexAttribute && type == att->attributeType())
                || name == att->name()) {
            return att;
        }
    }
    return nullptr;
}

void tst_gltfPlugins::compareAttributes(Qt3DRender::QAttribute *a1, Qt3DRender::QAttribute *a2)
{
    QCOMPARE(a1 == nullptr, a2 == nullptr);
    if (a1) {
        QCOMPARE(a1->attributeType(), a2->attributeType());
        QCOMPARE(a1->vertexBaseType(), a2->vertexBaseType());
        QCOMPARE(a1->vertexSize(), a2->vertexSize());
        QCOMPARE(a1->count(), a2->count());
    }
}

Qt3DCore::QEntity *tst_gltfPlugins::findCameraChild(Qt3DCore::QEntity *entity,
                                                    Qt3DRender::QCameraLens::ProjectionType type)
{
    for (auto child : entity->children()) {
        if (auto childEntity = qobject_cast<Qt3DCore::QEntity *>(child)) {
            for (auto component : childEntity->components()) {
                if (auto cameraLens = qobject_cast<Qt3DRender::QCameraLens *>(component)) {
                    if (cameraLens->projectionType() == type)
                        return childEntity;
                }
            }
            if (auto cameraEntity = findCameraChild(childEntity, type))
                return cameraEntity;
        }
    }
    return nullptr;
}

void tst_gltfPlugins::exportAndImport_data()
{
    QTest::addColumn<bool>("binaryJson");
    QTest::addColumn<bool>("compactJson");

    QTest::newRow("No options") << false << false;
#ifndef VISUAL_CHECK
    QTest::newRow("Binary json") << true << false;
    QTest::newRow("Compact json") << false << true;
    QTest::newRow("Binary/Compact json") << true << true; // Compact is ignored in this case
#endif
}

void tst_gltfPlugins::exportAndImport()
{
    QFETCH(bool, binaryJson);
    QFETCH(bool, compactJson);

    createTestScene();

#ifdef PRESERVE_EXPORT
    m_exportDir->setAutoRemove(false);
    qDebug() << "Export Directory:" << m_exportDir->path();
#endif

    const QString sceneName = QStringLiteral("MyGLTFScene");
    const QString exportDir = m_exportDir->path();

    // Export the created scene using GLTF export plugin
    QStringList keys = Qt3DRender::QSceneExportFactory::keys();
    for (const QString &key : keys) {
        Qt3DRender::QSceneExporter *exporter =
                Qt3DRender::QSceneExportFactory::create(key, QStringList());
        if (exporter != nullptr && key == QStringLiteral("gltfexport")) {
            QVariantHash options;
            options.insert(QStringLiteral("binaryJson"), QVariant(binaryJson));
            options.insert(QStringLiteral("compactJson"), QVariant(compactJson));
            exporter->exportScene(m_sceneRoot1, exportDir, sceneName, options);
            break;
        }
    }

    QCoreApplication::processEvents();

    // Import the exported scene using GLTF import plugin
    Qt3DCore::QEntity *importedScene = nullptr;
    keys = Qt3DRender::QSceneImportFactory::keys();
    for (auto key : keys) {
        Qt3DRender::QSceneImporter *importer =
                Qt3DRender::QSceneImportFactory::create(key, QStringList());
        if (importer != nullptr && key == QStringLiteral("gltf")) {
            QString sceneSource = exportDir;
            if (!sceneSource.endsWith(QLatin1Char('/')))
                sceneSource.append(QLatin1Char('/'));
            sceneSource += sceneName;
            sceneSource += QLatin1Char('/');
            sceneSource += sceneName;
            sceneSource += QStringLiteral(".qgltf");
            importer->setSource(QUrl::fromLocalFile(sceneSource));
            importedScene = importer->scene();
            break;
        }
    }

    importedScene->setParent(m_sceneRoot2);

    // Compare contents of the original scene and the exported one.
    for (auto it = m_entityMap.begin(), end = m_entityMap.end(); it != end; ++it) {
        QString name = it.key();
        Qt3DCore::QEntity *exportedEntity = it.value();
        Qt3DCore::QEntity *importedEntity = findChildEntity(importedScene, name);
        QVERIFY(importedEntity != nullptr);
        if (importedEntity) {
            compareComponents(transformComponent(exportedEntity),
                              transformComponent(importedEntity));
            compareComponents(lightComponent(exportedEntity),
                              lightComponent(importedEntity));
            compareComponents(cameraComponent(exportedEntity),
                              cameraComponent(importedEntity));
            compareComponents(meshComponent(exportedEntity),
                              meshComponent(importedEntity));
            compareComponents(materialComponent(exportedEntity),
                              materialComponent(importedEntity));
        }
    }


#ifdef VISUAL_CHECK
    qDebug() << "Dumping original entity tree:";
    walkEntity(m_sceneRoot1, 0);
    qDebug() << "Dumping imported entity tree:";
    walkEntity(importedScene, 0);

    // Find the camera to actually show the scene
    m_view2->defaultFrameGraph()->setCamera(
                findCameraChild(m_sceneRoot2, Qt3DRender::QCameraLens::OrthographicProjection));
    QTest::qWait(VISUAL_CHECK);

    m_view1->defaultFrameGraph()->setCamera(
                findCameraChild(m_sceneRoot1, Qt3DRender::QCameraLens::PerspectiveProjection));
    m_view2->defaultFrameGraph()->setCamera(
                findCameraChild(m_sceneRoot2, Qt3DRender::QCameraLens::PerspectiveProjection));
    QTest::qWait(VISUAL_CHECK);
#endif
}

QTEST_MAIN(tst_gltfPlugins)

#include "tst_gltfplugins.moc"
